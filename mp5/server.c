#include "http.h"
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>


void *client_thread(void *vptr) {
    int fd = *((int *)vptr);
    HTTPRequest req;
    
    // 1. Parse the client's HTTP request
    if (httprequest_read(&req, fd) < 0) {
        close(fd);
        return NULL;
    }
    char *file_path = req.path;
    if (strcmp(file_path, "/") == 0) {
        file_path = "/index.html";
    }
    char full_path[1024];
    sprintf(full_path, "static%s", file_path);
    struct stat st;
    if (stat(full_path, &st) == -1) {
        char *not_found_msg = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        write(fd, not_found_msg, strlen(not_found_msg));
    } else {
        char header[2048];
        int file_fd = open(full_path, O_RDONLY);
        char *content_type = NULL;
        if (strstr(file_path, ".png")) {
            content_type = "image/png";
        } else if (strstr(file_path, ".html")) {
            content_type = "text/html";
        }
        sprintf(header, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n", st.st_size, content_type);
        write(fd, header, strlen(header));
        char buffer[4096];
        ssize_t bytes_read;
        while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
            write(fd, buffer, bytes_read);
        }
        close(file_fd);
    }
    close(fd);
    return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    return 1;
  }
  int port = atoi(argv[1]);
  printf("Binding to port %d. Visit http://localhost:%d/ to interact with your server!\n", port, port);

  // socket:
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // bind:
  struct sockaddr_in server_addr, client_address;
  memset(&server_addr, 0x00, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);  
  bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));

  // listen:
  listen(sockfd, 10);

  // accept:
  socklen_t client_addr_len;
  while (1) {
    int *fd = malloc(sizeof(int));
    client_addr_len = sizeof(struct sockaddr_in);
    *fd = accept(sockfd, (struct sockaddr *)&client_address, &client_addr_len);
    printf("Client connected (fd=%d)\n", *fd);

    pthread_t tid;
    pthread_create(&tid, NULL, client_thread, fd);
    pthread_detach(tid);
  }

  return 0;
}