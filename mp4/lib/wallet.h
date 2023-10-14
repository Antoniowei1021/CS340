#pragma once
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct resource_t {
    char* name;
    struct resource_t* next;
    int count;
} resource_t;
typedef struct wallet_t_ {
  // Add anything here! :)
    resource_t * head;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} wallet_t;



void wallet_init(wallet_t *wallet);
int wallet_get(wallet_t *wallet, const char *resource);
int wallet_change_resource(wallet_t *wallet, const char *resource, const int delta);
void wallet_destroy(wallet_t *wallet);

#ifdef __cplusplus
}
#endif