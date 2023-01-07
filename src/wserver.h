#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "common.h"
#include "common_threads.h"
// don't include pc_header.h b/c it's already incorporated in 
#include "request.h"

#include "io_helper.h"
#include "buffer.h"


// initlize the cv/locking mechanisms 
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


char default_root[] = ".";
char default_schedalg[] = "FIFO"; 

// default values 
char *root_dir = default_root;
int port = 10000;
int producer_threads = 1; // always just one I think 
int consumer_threads = 1; // consumers, always just one producer 
int bufferSize = 1; 
char *schedalg = default_schedalg; 

int listen_fd; // networking / port stuff 

Buffer* buffer; 
pthread_t master; 
pthread_t* workers; 

/**
 * @brief start up the master and workers 
 * 
 */
void createAllThreads(); 

/**
 * @brief join threads back in 
 * 
 * @param numWorkers 
 */
void killWorkerThreads(int numWorkers); 

/**
 * @brief function for the master thread (producer) to call 
 * 
 * @param arg 
 * @return void* 
 */
void *producer(void *arg); 

/**
 * @brief function for the worker thread (consumers) to call 
 * 
 * @param arg 
 * @return void* 
 */
void *consumer(void *arg); 

/**
 * @brief add to the buffer, helper function for producer 
 * 
 * @param value 
 * @return true 
 * @return false 
 */
bool put(int value); 

/**
 * @brief pop from the buffer, helper function for the consumer 
 * 
 * @return Node* 
 */
Node* get(); 