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
    if (!req || !buffer || buffer_len <= 0) {
        return -1;
    }
    char *current = buffer;
    char *end = buffer + buffer_len;
    char *space1 = strchr(current, ' ');
    if (!space1 || space1 >= end) return -1;
    req->action = strndup(current, space1 - current);
    current = space1 + 1;
    char *space2 = strchr(current, ' ');
    if (!space2 || space2 >= end) return -1;
    req->path = strndup(current, space2 - current);
    current = space2 + 1;
    char *newline = strchr(current, '\r');
    if (!newline || newline >= end) return -1;
    req->version = strndup(current, newline - current);
    current = newline + 2; 
    _Header *last_header = NULL;
    while (current < end) {
        if (*current == '\r' && *(current + 1) == '\n') {
            current += 2;
            break;
        }
        char *colon = strchr(current, ':');
        if (!colon) return -1;
        char *header_key = strndup(current, colon - current);
        current = colon + 1;
        while (*current == ' ') current++;
        newline = strchr(current, '\r');
        if (!newline) return -1;
        char *header_value = strndup(current, newline - current);
        current = newline + 2; // skip "\r\n"
        _Header *new_header = malloc(sizeof(_Header));
        new_header->key = header_key;
        new_header->value = header_value;
        new_header->next = NULL;
        if (!req->head) {
            req->head = new_header;
        } else {
            last_header->next = new_header;
        }
        last_header = new_header;
    }
    req->payload = current;
    return current - buffer;
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
     if (!req || !key) {
        return NULL;
    }
    _Header* it = req->head;
    while (it) {
        if (!strcmp(it->key, key)) {
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
    free((void*)req->action);
    free((void*)req->path);
    free((void*)req->version);

    _Header *current = req->head;
    while (current) {
        _Header *next = current->next;
        free(current->key);
        free(current->value);
        free(current);
        current = next;
    }
}