#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "http.h"

/**
 * httprequest_parse_headers
 * 
 * Populate a `req` with the contents of `buffer`, returning the number of bytes used from `buf`.
 */
#include <string.h>
ssize_t httprequest_parse_headers(HTTPRequest *req, char *buffer, ssize_t buffer_len) {
    req->head = NULL;
    char *current = buffer;
    char *end = buffer + buffer_len;
    char *space1 = strchr(current, ' ');
    req->action = strndup(current, space1 - current);
    current = space1 + 1;
    char *space2 = strchr(current, ' ');
    req->path = strndup(current, space2 - current);
    current = space2 + 1;
    char *newline = strchr(current, '\r');
    req->version = strndup(current, newline - current);
    current = newline + 2; 
    _Header *last_header = NULL;
    while (current < end) {
        if (*current == '\r' && *(current + 1) == '\n') {
            current += 2;
            break;
        }
        char *colon = strchr(current, ':');
        char *header_key = strndup(current, colon - current);
        current = colon + 1;
        while (*current == ' ') current++;
        newline = strchr(current, '\r');
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
    char buffer[4096];
    char *temp_buffer = NULL;
    ssize_t bytes_read, total_bytes = 0;
    ssize_t temp_buffer_len = 0;
    while (1) {
        bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            free(temp_buffer);
            return -1;  
        }
        buffer[bytes_read] = '\0'; 
        temp_buffer = realloc(temp_buffer, temp_buffer_len + bytes_read + 1);
        memcpy(temp_buffer + temp_buffer_len, buffer, bytes_read);
        temp_buffer_len += bytes_read;
        temp_buffer[temp_buffer_len] = '\0';
        if (strstr(temp_buffer, "\r\n\r\n")) {
            break;
        }
    }
    if (httprequest_parse_headers(req, temp_buffer, temp_buffer_len) < 0) {
        free(temp_buffer);
        return -1; 
    }
    total_bytes += temp_buffer_len;
    _Header *header = req->head;
    int content_length = -1; 
    while (header) {
        if (strcasecmp(header->key, "Content-Length") == 0) {
            content_length = atoi(header->value);
            break;
        }
        header = header->next;
    }
    if (content_length == -1 || content_length < 0) {
        req->payload = NULL; 
        free(temp_buffer);
        return total_bytes;
    }
    if (content_length > 0) {
        char *payload_start_in_buffer = strstr(temp_buffer, "\r\n\r\n") + 4;
        int payload_bytes_already_read = temp_buffer_len - (payload_start_in_buffer - temp_buffer);
        int payload_bytes_to_read = content_length - payload_bytes_already_read;
        while (payload_bytes_to_read > 0) {
            bytes_read = read(sockfd, buffer, sizeof(buffer) - 1 < payload_bytes_to_read ? sizeof(buffer) - 1 : payload_bytes_to_read);
            if (bytes_read <= 0) {
                free(temp_buffer);
                return -1; 
            }
            temp_buffer = realloc(temp_buffer, temp_buffer_len + bytes_read + 1);
            memcpy(temp_buffer + temp_buffer_len, buffer, bytes_read);
            temp_buffer_len += bytes_read;
            temp_buffer[temp_buffer_len] = '\0';
            total_bytes += bytes_read;
            payload_bytes_to_read -= bytes_read;
        }
    }
    if (content_length > 0) {
        req->payload = temp_buffer + (strstr(temp_buffer, "\r\n\r\n") + 4 - temp_buffer);
    } else {
        req->payload = NULL;
    }
    return total_bytes;
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
    // free((void*)req->action);
    // free((void*)req->path);
    // free((void*)req->version);
    // free((void*)req->payload);
    // _Header *current = req->head;
    // while (current) {
    //     _Header *next = current->next;
    //     free(current->key);
    //     free(current->value);
    //     free(current);
    //     current = next;
    // }
}