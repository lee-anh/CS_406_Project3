/********************I HAVE MADE MODIFCIATIONS TO THIS CLIENT***********************/

//
// client.c: A very, very primitive HTTP client.
// NOTE: the original wclient has been moved to original_wclient.c 
// 
// This client is multithreaded and simulates what it would be like if many requests
// were sent to the server at once
// 
// To run, try: 
//      client hostname portnumber numFiles numClientThreads 
// 
// The program will prompt you for numFiles files names to make requests to the server for
// After loading in these files it creates the client threads and RANDOMLY selects input to run
// 
// Additionally, I have made feed.txt so that you can use input redirction 
// ex. ./wclient 139.147.9.169 10002 8 10 < feed.txt
//




#include "io_helper.h"
#include "common.h"
#include "common_threads.h"
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define MAXBUF (8192)
#define EXTRASPACE (255)

int loops = 1; // increase simutaenous count by a factor of this amount (increase the load)

int numClients;  // how many simutaneous requests to send 

int numFiles = 0; 
char** files; 
char *host, *filename;
int port;

    
pthread_t* client_threads; 

//
// Send an HTTP request for the specified file 
//
void client_send(int fd, char *filename) {
    char buf[2*MAXBUF + EXTRASPACE];
    char minibuf[MAXBUF]; 
    char hostname[MAXBUF];
    
    gethostname_or_die(hostname, MAXBUF);
    
    /* Form and send the HTTP request */
    sprintf(minibuf, "GET %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", minibuf, hostname);
    write_or_die(fd, buf, strlen(buf));
}

//
// Read the HTTP response and print it out
//
void client_print(int fd) {
    char buf[MAXBUF];  
    int n;
    
    // Read and display the HTTP Header 
    n = readline_or_die(fd, buf, MAXBUF);
    while (strcmp(buf, "\r\n") && (n > 0)) {
	printf("Header: %s", buf);
	n = readline_or_die(fd, buf, MAXBUF);
	
	// If you want to look for certain HTTP tags... do I?
	// int length = 0;
	//if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
	//    printf("Length = %d\n", length);
	//}
    }
    
    // Read and display the HTTP Body 
    n = readline_or_die(fd, buf, MAXBUF);
    while (n > 0) {
	printf("%s", buf);
	n = readline_or_die(fd, buf, MAXBUF);
    }
}

/**
 * @brief opens port, randomly selects file for thread and sends it to the server
 * for the client threads to call  
 * 
 * @param arg not used 
 * @return void*  NULL 
 */
void *client_produce(void *arg){
    for(int i = 0; i < loops; i++){
        
        int clientfd = open_client_fd_or_die(host, port);
        char* filenombre = files[rand() % numFiles]; 
        client_send(clientfd, filenombre);
        client_print(clientfd);
        close_or_die(clientfd);
        
    }
    return NULL; 
}


/**
 * @brief creates client threads 
 * 
 * @param num number of client threads to create  
 */
void create_client_threads(int num){
    client_threads = malloc(num*sizeof(pthread_t)); 
    for(int i = 0; i < num; i++){
        Pthread_create(&client_threads[i], NULL, client_produce, (void *) (long long) i);
    }
}

/**
 * @brief main for running a multi threaded client
 * 
 * @param argc number of commmandline arguments 
 * @param argv commandline arguments 
 * @return int 
 */
int main(int argc, char *argv[]) {
    if (argc != 5) {
	fprintf(stderr, "Usage: %s <host> <port> <number of files> <number of client threads>\n", argv[0]);
	exit(1);
    }
    
    host = argv[1];
    port = atoi(argv[2]);
    numFiles = atoi(argv[3]);
    numClients = atoi(argv[4]); 


    if(numFiles > 8){
        numFiles = 8; // just because I only have 8 files in feed.txt :) 
    }



   // printf("Num files: %d", numFiles); 

    srand(time(NULL)); 

    // can maybe use input redirection and then you'll have both options
    // get all of the file names 

    // prompts user for filenames 
    files = calloc(numFiles, sizeof(char*)); 
    for(int i = 0; i < numFiles; i++){
        files[i] = calloc(255, sizeof(char)); 
        printf("Enter filename: ");
        scanf(" %255[^\n]s", files[i]);  

    }

    // create threads 
    create_client_threads(numClients); 

    // join threads 
    for(int i = 0; i < numClients; i++){
		pthread_join(client_threads[i], NULL);
	}

    // clean up the memory 
    for(int i = 0; i < numFiles; i++){
        free(files[i]); 
    }
    free(files); 

    exit(0);
}