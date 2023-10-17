#include "http.h"
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
void client_thread(int fd) {
    // 1. Read the HTTP Request
    HTTPRequest req;
    if (httprequest_read(&req, fd) < 0) {
        // Handle error
        write(fd, "HTTP/1.1 400 Bad Request\r\n\r\n", 28);
        close(fd);
        return;
    }

    // 2. Process the Request Path
    const char *path = req.path;
    if (strcmp(path, "/") == 0) {
        path = "/index.html";
    }

    // Construct the full file path
    char filepath[1024]; // or some max path length
    snprintf(filepath, sizeof(filepath), "static%s", path);

    // 3. Check if the Requested File Exists
    if (access(filepath, F_OK) == -1) {
        // Send 404 Not Found response
        write(fd, "HTTP/1.1 404 Not Found\r\n\r\n", 27);
        close(fd);
        return;
    }

    // 4. Determine the Content-Type
    char *content_type = "application/octet-stream"; // default
    if (strstr(filepath, ".html")) {
        content_type = "text/html";
    } else if (strstr(filepath, ".png")) {
        content_type = "image/png";
    }

    // 5. Construct and Send the HTTP Response

    // Open file for reading
    int file_fd = open(filepath, O_RDONLY);
    if (file_fd == -1) {
        write(fd, "HTTP/1.1 500 Internal Server Error\r\n\r\n", 37);
        close(fd);
        return;
    }

    struct stat st;
    fstat(file_fd, &st);
    ssize_t file_size = st.st_size;

    char response_header[1024];
    int header_len = snprintf(response_header, sizeof(response_header),
                              "HTTP/1.1 200 OK\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %zd\r\n\r\n",
                              content_type, file_size);
    write(fd, response_header, header_len);

    // Transfer file contents
    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
        write(fd, buffer, bytes_read);
    }

    // 6. Clean Up and Close
    close(file_fd);
    close(fd);
    // You may also need to free the resources of the HTTPRequest (like headers) here if necessary
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