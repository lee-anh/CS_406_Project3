#include "buffer.h" 


/**
 * @brief Create an empty  Buffer object
 * dynamically allocates space for the buffer 
 * initializes number of elements to 0
 * 
 * @param maxNumElements set a max limit to the number of elements 
 * @return Buffer* buffer that was created
 */
Buffer* createBuffer(int maxNumElements){
    Buffer* buffer = malloc(sizeof(Buffer)); 
    buffer->head = NULL; 
    buffer->tail = NULL; 
    buffer->capacity = maxNumElements; 
    buffer->numElements = 0; 

    return buffer; 
}

/**
 * @brief Create a Node object helper function to adding to the buffer 
 * method, uri, version are strdup'd so they become dynamically allocated here 
 * next and prev are left NULL because they are set in the calling function
 * 
 * @param filedescriptor identifier for file you are adding 
 * @param method networking stuff 
 * @param uri networking stuff 
 * @param version networking stuff 
 * @param fileSize used as a comparator for sorting  
 * @return Node* 
 */
Node* createNode(int filedescriptor, char* method, char* uri, char* version, off_t fileSize){
    // need to dyanmically allocate this 
    Node* thisNode = malloc(sizeof(Node));
    thisNode->next = NULL;
    thisNode->prev = NULL;
    thisNode->data = filedescriptor; 
    thisNode->stat = fileSize; 
    thisNode->method = strdup(method); 
    thisNode->uri = strdup(uri); 
    thisNode->version = strdup(version); 
    return thisNode; 
}

/**
 * @brief add a client request to the buffer, use for FIFO implementation
 *  treats the buffer like a queue 
 *  handles cases of inserting to an empty buffer, inserting to the end of the buffer queue 
 *  also handles if you try to insert to a buffer at capacity 
 * 
 * @param buffer to add to 
 * @param elementToAdd a file descriptor 
 * @param method networking stuff
 * @param uri networking stuff
 * @param version networking stuff 
 * @return true if add was successful
 * @return false if buffer was at capacity 
 */
bool addToBuffer(Buffer* buffer, int elementToAdd, char* method, char* uri, char* version){

    if(buffer->numElements == buffer->capacity){
        return false; 
    }

    Node* thisNode = createNode(elementToAdd, method, uri, version, 0); 

    // first element in the Buffer
    if(buffer->numElements == 0){
        buffer->head = thisNode; 
        buffer->tail = thisNode; 
    } else {
        thisNode->prev = buffer->tail; // set double link 
        buffer->tail->next = thisNode; 
        buffer->tail = thisNode; // thisNode is now the tail 
    }


    buffer->numElements += 1; 
    //printf("Just added: %d\n", buffer->tail); 
    //printBuffer(buffer); 
    return true; 
}


/**
 * @brief add a client request to the buffer, use for SFF implementation
 * ordered insertion: smallest file to largest file 
 * 
 * handles cases of trying to insert into a buffer at capacity
 * traverse the linked list to find the correct place to insert 
 * handles insertion at the head, body, and end of the linked list 
 * @param buffer to add to 
 * @param elementToAdd a file descriptor
 * @param method networking stuff
 * @param uri networking stuff
 * @param version networking stuff
 * @param fileSize comparator to sort 
 * @return true if add was successful
 * @return false if buffer was at capacity 
 */
bool addToBufferOrdered(Buffer* buffer, int elementToAdd, char* method, char* uri, char* version, off_t fileSize){
    // must traverse to find the correct location
    if(buffer->numElements == buffer->capacity) {
        return false; 
    }
    
    // create Node to insert 
    Node* thisNode = createNode(elementToAdd, method, uri, version, fileSize); 

    // first element in the buffer 
    if(buffer->numElements == 0){
        buffer->head = thisNode;
        buffer->tail = thisNode; 
        buffer->numElements += 1; 
    } else {
        Node* current = buffer->head; 

        while(true){ 
           
            if(current->stat >= thisNode->stat){ // current is the first element, you are inserting the new head 
                
                if(current == buffer->head){
                    // link 
                    thisNode->next = current; 
                    current->prev = thisNode;

                    // reassign head 
                    buffer->head = thisNode; 
                    
                } else { // current is not the first element, body insert 

                    // link 
                    current->prev->next = thisNode;
                    thisNode->prev = current->prev; 

                    current->prev = thisNode;
                    thisNode->next = current; 
                    
                }

                buffer->numElements += 1;
            
                break; 
            }

            // last chance to get on -- you are the new tail 
            if(current == buffer->tail){ // critical that this is assigned correctly 
                // link 
                current->next = thisNode;
                thisNode->prev = current; 

                // reassign tail 
                buffer->tail = thisNode; 
                buffer->numElements += 1;
                
                break; 

            }

            // if current->stat is less than thisNode->stat we simply move on to the next element 
            current = current->next; 

        }
        
    }

  //  printBuffer(buffer); 

    return true; 


}

/**
 * @brief get and remove the head of the buffer 
 *  if FIFO, will be the correct node (first in)
 *  if SFF, will be the correct node (smalleset file size)
 * @param buffer to pop from 
 * @return Node* the head 
 */
Node* popElement(Buffer* buffer){
    if(buffer->numElements == 0){
        return NULL; 
    } 

    Node* toReturn = buffer->head; 
    if(buffer->numElements == 1){
        // just one element, it's the head and the tail 
        //Node* temp = buffer->head; 
        buffer->head = NULL;
        buffer->tail = NULL; 
        //free(temp); 
        
        
    } else {
        // more than one element 
        //Node* temp = buffer->head; 
        buffer->head = buffer->head->next;
        buffer->head->prev = NULL; 
        //free(temp); 
    }

    buffer->numElements -= 1; // decrement count
  //  printBuffer(buffer); 
    

    return toReturn; 
}


/**
 * @brief prints contents of buffer, helper function
 * traverses the linked list and prints out the contents of each node 
 * 
 * @param toPrint 
 */
void printBuffer(Buffer* toPrint){
    Node* currentNode = toPrint->head; 
    if(currentNode == NULL){
        printf("empty buffer\n");
        return;  
    }

    printf("buffer (numElements: %d, capacity: %d): \n", toPrint->numElements, toPrint->capacity); 
    while(true){
        printf("fd: %d, ", currentNode->data); 
        printf("method: %s, uri %s, version: %s", currentNode->method, currentNode->uri, currentNode->version); 
        printf("size%9jd\n", currentNode->stat);
        currentNode = currentNode->next; 
        if(currentNode == NULL){
            break; 
        }
    }
    printf("\n"); // endl character 
}



/**
 * @brief free memory of a node
 * can be called as a helper function or by itself (if the node has been popped)
 * 
 * @param node 
 */
void nodeDestructor(Node* node){
    // free the strings 
    if(node->version != NULL) free(node->version); 
    if(node->uri != NULL) free(node->uri);
    if(node->method != NULL) free(node->method);

    free(node); // free the actual node 
}


/**
 * @brief free memory of a buffer 
 * traverses linked list and frees each node inside 
 * 
 * @param toDestroy 
 */
void bufferDestructor(Buffer* toDestroy){
    Node* current = toDestroy->head; 
    Node* next; 
    while(true){
        next = current->next; 

        nodeDestructor(current); 
        //free(current);

        current = next; 
        if(current == NULL){
            break; 
        }
    }
    free(toDestroy); 
}
