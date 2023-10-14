#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "lib/wallet.h"

// Your server must use only one global wallet:
wallet_t wallet;
void *client_communication_thread(void *vptr_fd) {
    int fd = *((int *)vptr_fd);
    char buffer[4096];

    while (1) {
        ssize_t len = recv(fd, buffer, 4096, 0);
        if (len <= 0) {
            printf("[%d]: socket closed or error occurred\n", fd);
            break;
        }
        
        // Remove all carriage return characters
        for (int i = 0, j = 0; i < len; i++) {
            if (buffer[i] != '\r') {
                buffer[j++] = buffer[i];
            }
        }
        buffer[len] = '\0';
        
        // Handling GET command
        if (strncmp(buffer, "GET", 3) == 0) {
            char resource_name[256];
            sscanf(buffer, "GET %s", resource_name);
            int count = wallet_get(&wallet, resource_name); // Assuming wallet is globally defined

            sprintf(buffer, "%d\n", count);
            send(fd, buffer, strlen(buffer), 0);

        } 
        // Handling MOD command
        else if (strncmp(buffer, "MOD", 3) == 0) {
            char resource_name[256];
            int delta;
            sscanf(buffer, "MOD %s %d", resource_name, &delta);

            // Trying to modify the resource and if resource goes negative, block until it's positive again.
            pthread_mutex_lock(&(wallet.mutex));
            while (wallet_change_resource(&wallet, resource_name, delta) < 0) {
                pthread_cond_wait(&(wallet.condition), &(wallet.mutex));
            }
            int count = wallet_get(&wallet, resource_name);
            pthread_mutex_unlock(&(wallet.mutex));

            sprintf(buffer, "%d\n", count);
            send(fd, buffer, strlen(buffer), 0);

        } 
        // Handling EXIT command
        else if (strncmp(buffer, "EXIT", 4) == 0) {
            break;
        }
    }

    close(fd);
    free(vptr_fd);
    return NULL;
}



void create_wallet_server(int port) {
  // Implement `create_wallet_server`
  // (You will need to use threads, which requires adding additional functions.)

  // socket:
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd <= 0) { perror("socket"); return; }
  printf("socket: returned fd=%d\n", sockfd);

  // bind:
  struct sockaddr_in server_addr;
  memset(&server_addr, 0x00, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);  
  if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
    perror("bind");
    return;
  }
  printf("bind: binding fd=%d to port %d (:%d)\n", sockfd, port, port);

  // listen:
  if (listen(sockfd, 10) != 0) {
    perror("listen");
    return;
  }
  printf("listen: fd=%d is now listening for incoming connections\n", sockfd);

  // continue to accept new connections forever:
  while (1) {
    struct sockaddr_in client_address;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);

    // accept:
    int *fd = malloc(sizeof(int));
    *fd = accept(sockfd, (struct sockaddr *)&client_address, &client_addr_len);
    
    char *ip = inet_ntoa(client_address.sin_addr);
    printf("accept: new client connected from %s, communication on fd=%d\n", ip, *fd);

    pthread_t tid;
    pthread_create(&tid, NULL, client_communication_thread, fd);
  }
  return;
}
int main(int argc, char* argv[]) {
  int c;
  int local_port = 34000;

  // Reads the (optional) command line argument:
  while((c = getopt(argc, argv, "p:")) != -1) {
    switch(c) {
      case 'p':
        if(optarg != NULL) {
          local_port = atoi(optarg);
        }
        break;
    }
  }
  // Calls `wallet_init`:
  wallet_init(&wallet);

  // Calls the `create_wallet_server` with the user-supplied port:
  printf("Launching wallet server on :%d\n", local_port);
  create_wallet_server(local_port);
}
