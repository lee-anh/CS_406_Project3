#ifndef __REQUEST_H__

#include "buffer.h"

void request_handle(Node* element);


//
// Return 1 if static, 0 if dynamic content
// Calculates filename (and cgiargs, for dynamic) from uri
//
int request_parse_uri(char *uri, char *filename, char *cgiargs); 

#endif // __REQUEST_H__
 