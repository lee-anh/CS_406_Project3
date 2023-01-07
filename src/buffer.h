#ifndef __BUFFER__
#define __BUFFER__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <sys/types.h>
#include <string.h>

// Buffer - a Linked List under the hood

/**
 * @brief Node object to be used inside the Linked list 
 * 
 */
typedef struct Node_struct{
    struct Node_struct* next; // doubly linked list 
    struct Node_struct* prev;
    int data; 
    char* method; // strings should be dynamically allocated 
    char* uri; 
    char* version; 
    off_t stat; 
} Node; 


/**
 * @brief Doubly Linked list, can keep track of head and tail Nodes 
 * 
 */
typedef struct Buffer_struct{
    Node* head;
    Node* tail; 
    int capacity;
    int numElements;

} Buffer; 


/**
 * @brief Create a Buffer object
 * 
 * @param maxNumElements set a max limit to the number of elements 
 * @return Buffer* buffer that was created
 */
Buffer* createBuffer(int maxNumElements);

/**
 * @brief Create a Node object helper function to adding to the buffer 
 * 
 * @param filedescriptor identifier for file you are adding 
 * @param method networking stuff 
 * @param uri networking stuff 
 * @param version networking stuff 
 * @param fileSize used as a comparator for sorting  
 * @return Node* 
 */
Node* createNode(int filedescriptor,  char* method, char* uri, char* version, off_t fileSize); 

/**
 * @brief add a client request to the buffer, use for FIFO implementation
 * 
 * @param buffer to add to 
 * @param elementToAdd a file descriptor 
 * @param method networking stuff
 * @param uri networking stuff
 * @param version networking stuff 
 * @return true if add was successful
 * @return false if buffer was at capacity 
 */
bool addToBuffer(Buffer* buffer, int elementToAdd, char* method, char* uri, char* version); 

/**
 * @brief add a client request to the buffer, use for SFF implementation
 * ordered insertion: smallest file to largest file 
 * @param buffer to add to 
 * @param elementToAdd a file descriptor
 * @param method networking stuff
 * @param uri networking stuff
 * @param version networking stuff
 * @param fileSize comparator to sort 
 * @return true if add was successful
 * @return false if buffer was at capacity 
 */
bool addToBufferOrdered(Buffer* buffer, int elementToAdd,  char* method, char* uri, char* version, off_t fileSize); 

/**
 * @brief get and remove the head of the buffer 
 * 
 * @param buffer to pop from 
 * @return Node* the head 
 */
Node* popElement(Buffer* buffer);


/**
 * @brief prints contents of buffer, helper function
 * 
 * @param toPrint 
 */
void printBuffer(Buffer* toPrint); 

/**
 * @brief free memory of a node 
 * 
 * @param node 
 */
void nodeDestructor(Node* node); 

/**
 * @brief free memory of a buffer 
 * 
 * @param toDestroy 
 */
void bufferDestructor(Buffer* toDestroy); 




#endif
