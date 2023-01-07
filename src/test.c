#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    int num = 0; 
    char *buf;
    if ((buf = getenv("QUERY_STRING")) != NULL) {
	    // just expecting a single number
	    num = (double) atoi(buf);
    }

    if(num < 0){
        printf("argument must be greater than or equal to 0"); 
        return(-1); 
    }
    for(int i = 0; i < num; i++){
        printf("hello\n"); 
    }
    return(0); 
    
}