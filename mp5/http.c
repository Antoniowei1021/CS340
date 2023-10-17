#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "http.h"
#include <sys/socket.h>

/**
 * httprequest_parse_headers
 * 
 * Populate a `req` with the contents of `buffer`, returning the number of bytes used from `buf`.
 */
#include <string.h>
ssize_t httprequest_parse_headers(HTTPRequest *req, char *buffer, ssize_t buffer_len) {
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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

ssize_t httprequest_read(HTTPRequest *req, int sockfd) {
    char buffer[BUFFER_SIZE];
    char *temp_buffer = NULL;
    ssize_t bytes_read, total_bytes = 0;
    ssize_t temp_buffer_len = 0;

    while (1) {
        bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            free(temp_buffer);
            return -1;  // error or socket closed
        }

        buffer[bytes_read] = '\0';  // Null-terminate the buffer

        // Append to temp_buffer
        temp_buffer = realloc(temp_buffer, temp_buffer_len + bytes_read + 1);
        memcpy(temp_buffer + temp_buffer_len, buffer, bytes_read);
        temp_buffer_len += bytes_read;
        temp_buffer[temp_buffer_len] = '\0';

        // Check if headers are done (presence of "\r\n\r\n")
        if (strstr(temp_buffer, "\r\n\r\n")) {
            break;
        }
    }

    // At this point, we've got the headers, and maybe some payload. We can now parse the headers.
    if (httprequest_parse_headers(req, temp_buffer, temp_buffer_len) < 0) {
        free(temp_buffer);
        return -1;  // error in parsing
    }

    // Update total_bytes to represent how much we've read so far.
    total_bytes += temp_buffer_len;

    // Check for the Content-Length header to see if there's more payload to read
    _Header *header = req->head;
    int content_length = -1;  // Initialize to -1 to detect if Content-Length is absent
    while (header) {
        if (strcasecmp(header->key, "Content-Length") == 0) {
            content_length = atoi(header->value);
            break;
        }
        header = header->next;
    }

    // Handle cases where Content-Length is absent or invalid
    if (content_length == -1 || content_length < 0) {
        req->payload = NULL;  // Set payload to NULL as specified in the test cases
        free(temp_buffer);
        return total_bytes;
    }

    // If Content-Length exists and is greater than 0, read that amount of payload
    if (content_length > 0) {
        char *payload_start_in_buffer = strstr(temp_buffer, "\r\n\r\n") + 4;
        int payload_bytes_already_read = temp_buffer_len - (payload_start_in_buffer - temp_buffer);
        int payload_bytes_to_read = content_length - payload_bytes_already_read;

        while (payload_bytes_to_read > 0) {
            bytes_read = read(sockfd, buffer, sizeof(buffer) - 1 < payload_bytes_to_read ? sizeof(buffer) - 1 : payload_bytes_to_read);
            if (bytes_read <= 0) {
                free(temp_buffer);
                return -1;  // error or socket closed
            }
            
            temp_buffer = realloc(temp_buffer, temp_buffer_len + bytes_read + 1);
            memcpy(temp_buffer + temp_buffer_len, buffer, bytes_read);
            temp_buffer_len += bytes_read;
            temp_buffer[temp_buffer_len] = '\0';

            total_bytes += bytes_read;
            payload_bytes_to_read -= bytes_read;
        }
    }

    // Make sure to assign payload only if there's actual content
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