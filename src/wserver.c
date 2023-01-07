
#include "wserver.h"



// ./wserver [-d <basedir>] [-p <portnum>] 
// 
/**
 * @brief main function
 * processes the flags from the command line 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[]) {
    int c;
    
    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1)
	switch (c) {
	case 'd':
		printf("OPTARG d: %s, len: %ld\n", optarg, strlen(optarg)); 
	    root_dir = optarg;
		if(strstr(root_dir, "..") != NULL){
			fprintf(stderr, "File Access cannot be above this directory. Do not have .. in pathname\n"); 
			exit(1); 
		} 
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 't':
		consumer_threads = atoi(optarg); 
		break; 

	case 'b':
		bufferSize = atoi(optarg); 
		break; 
	case 's':
		schedalg = optarg; 
		break; 
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b bufferSize] [-s schedulingAlgorithm]\n");
	    exit(1);
	}


    // run out of this directory
    chdir_or_die(root_dir);
	buffer = createBuffer(bufferSize); 
	listen_fd = open_listen_fd_or_die(port);
	createAllThreads(); 

	killWorkerThreads(consumer_threads);
	
    return 0;
}


/**
 * @brief Start up the worker threads and the master thread 
 * Master thread = 1 producer
 * Worker threads = multiple consumers 
 * 
 */
void createAllThreads(){
	// start up worker threads 
	workers = malloc(sizeof(pthread_t)*consumer_threads); 
	for(int i = 0; i < consumer_threads; i++){
		Pthread_create(&workers[i], NULL, consumer, (void *) (long long) i); 
		//printf("Worker: %ld\n", workers[i]); 
	}
	
	// create the master thread
	Pthread_create(&master, NULL, producer, (void *) (long long) consumer_threads); 
}


/**
 * @brief joins the threads back into main thread
 *  (at program termination which never actually happens on this primitive server)
 * 
 * @param numWorkers 
 */
void killWorkerThreads(int numWorkers){
	Pthread_join(master, NULL); 
	for(int i = 0; i < numWorkers; i++){
		pthread_join(workers[i], NULL);
	}
	free(workers); // you can free them but they are all dead :`(

}

/**
 * @brief function for the master thread (producer to call)
 *  query from the client request queue. Atomically add to the buffer 
 *  sleep if the buffer is at capacity 
 * @param arg 
 * @return void* 
 */
void* producer(void *arg){
	//int fd = (int) arg; 
	int fd; 
	while (1) {
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		fd = conn_fd; 

		Mutex_lock(&mutex); 
		while(buffer->numElements == buffer->capacity){
			Cond_wait(&empty, &mutex);
	
		}
		bool res = put(fd); 
		if(res == false){
			printf("Blocked"); // just for debugging 
		}

		Cond_signal(&fill);

		Mutex_unlock(&mutex); 
    }


	return NULL; 

	
}


/**
 * @brief function for the worker thread (consumers) to call 
 *  atomically pop from the buffer 
 *  sleep if the buffer is empty 
 * 
 *  handle the request and close the connection
 * 
 * @param arg 
 * @return void* 
 */
void *consumer(void *arg){
	while(1){

	Mutex_lock(&mutex);
	
	while(buffer->numElements == 0){
		Cond_wait(&fill, &mutex);

	}
	Node* element = get(); 
	int fd = element->data; 
	Cond_signal(&empty);
	Mutex_unlock(&mutex);

	request_handle(element); 
	close_or_die(fd); 

	}

	return NULL; 
}

/**
 * @brief add to the buffer, helper function for producer
 * extracts the file name
 * caches method, uri, and version in the buffer 
 * 
 * @param value filedescriptor to add to the buffer 
 * @return true if value is successfully put in buffer
 * @return false if value is not successfully put in buffer (buffer is full)
 */
bool put(int value){
	bool toReturn = false; 
	int maxbuf = 8192; 
	char buf[maxbuf], method[maxbuf], uri[maxbuf], version[maxbuf];
	readline_or_die(value, buf, maxbuf);
	sscanf(buf, "%s %s %s", method, uri, version);


	
	if(strcmp(schedalg, default_schedalg) == 0){ // FIFO implementation
		toReturn = addToBuffer(buffer, value, method, uri, version); 
	
	} else { // SFF implementation 
		// just have to make it work for SFF now 
		char filename[maxbuf], cgiargs[maxbuf];
	

		// make a copy of the uri so you don't process the original string which will 
		// need to get freshly processed in request.c 
		char uricopy[maxbuf];
		strcpy(uricopy, uri); 
		
		request_parse_uri(uricopy, filename, cgiargs); //really just trying to get the filname 
		
		struct stat st1; 
		stat(filename, &st1);
	
		//printf("filename: %s, size%9jd\n", filename, st1.st_size);
		
		toReturn = addToBufferOrdered(buffer, value, method, uri, version, st1.st_size); 
		//toReturn = addToBufferOrdered(buffer, value, method, uri, version, 0); 
	
	}
	return toReturn; 
}


/**
 * @brief pop from the buffer, helper function for the consumer 
 * 
 * @return Node* 
 */
Node* get(){

	Node* element = popElement(buffer); 
	return element; 

}

