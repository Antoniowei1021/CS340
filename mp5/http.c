#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "http.h"


/**
 * httprequest_parse_headers
 * 
 * Populate a `req` with the contents of `buffer`, returning the number of bytes used from `buf`.
 */
#include <string.h>
ssize_t httprequest_parse_headers(HTTPRequest *req, char *buffer, ssize_t buffer_len) {
 return -1;
}



/**
 * httprequest_read
 * 
 * Populate a `req` from the socket `sockfd`, returning the number of bytes read to populate `req`.
 */
ssize_t httprequest_read(HTTPRequest *req, int sockfd) {
  return -1;
}


/**
 * httprequest_get_action
 * 
 * Returns the HTTP action verb for a given `req`.
 */
const char *httprequest_get_action(HTTPRequest *req) {
    return req->action;
}


/**
 * httprequest_get_header
 * 
 * Returns the value of the HTTP header `key` for a given `req`.
 */
const char *httprequest_get_header(HTTPRequest *req, const char *key) {
    _Header* it = req->head;
    while (it) {
        if (strcmp(it->key, key)) {
            return it->value;
        }
        it = it->next;
    }
    return NULL;
}


/**
 * httprequest_get_path
 * 
 * Returns the requested path for a given `req`.
 */
const char *httprequest_get_path(HTTPRequest *req) {
    return req->path;
}


/**
 * httprequest_destroy
 * 
 * Destroys a `req`, freeing all associated memory.
 */
void httprequest_destroy(HTTPRequest *req) {
    if (!req) {
        return; 
    }
    _Header *current = req->head;
    while (current) {
        _Header *next = current->next;
        
        free(current->key);  
        free(current->value); 
        free(current);       

        current = next;      
    }
    free(req);
}