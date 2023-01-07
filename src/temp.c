



#include <stdio.h>
#include <pthread.h>

#include "request.h"
#include "io_helper.h"
char default_root[] = ".";
//char default_schedalg[] = "FIFO"; 
pthread_t *threadArray; 


// the way I will distinguish my code from written code is I will use camelCasing 
 
//
// ./wserver [-d <basedir>] [-p <portnum>] [-t <num of threads>] [-b <num of buffers>] [-s <scheduling algorithm>]
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
	int threads = 1; 
	int buffers = 1; 
	//char *schedalg = default_schedalg; 

    
	// process the flags 
    while ((c = getopt(argc, argv, "d:p")) != -1)
	switch (c) {
	case 'd':
	    root_dir = optarg; // optarg is a char* variable that is set by the getopt 
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	/*
	case 't':
		port = atoi(optarg); 
		break; 
	case 'b':
		buffers = atoi(optarg); 
		break; 
	case 's':
		schedalg = optarg; 
		break; 
	*/
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t numThreads] [-b numBuffer] [-s schedulingAlgorithm]\n");
	    exit(1);
	}

    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);

    while (1) {
	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr);
	int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
	request_handle(conn_fd); // so I think this is where I should add the buffer and the threads 
	close_or_die(conn_fd);
    }
    return 0;
}

// where should all of the worker threads be created and wait? 
// should they go in an array of threads? 

// might have locks in thread creation?
// need to put it in a concurrent queue of requests? 

/*
// need to create the threads
void createThreads(int numOfThreads)
	threadArray = malloc(sizeof(pthread_t)* )
*/
// the buffer
	// workers need to wait if the buffer is empty
	// master needs to block requests and wait when the buffer is full 

	// requests are placed in buffer 

	// workers pick up requests

	// I think there a distinction between buffer in prod-consumer relationship
	// and THE BUFFER here because all threads stay in THE BUFFER
	// where as in the buffer a worker "consumes" a request 
	// THE BUFFER is the total number of requests the server can handle at any given point 
	

 
