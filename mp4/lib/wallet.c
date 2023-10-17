#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include "wallet.h"

/**
 * Initializes an empty wallet.
 */
void wallet_init(wallet_t *wallet) {
  // Implement `wallet_init`
    pthread_cond_init(&wallet->condition, NULL);
    pthread_mutex_init(&wallet->mutex, NULL);
    wallet->head = NULL;
}

/**
 * Returns the amount of a given `resource` in the given `wallet`.
 */
int wallet_get(wallet_t *wallet, const char *resource) {
  // Implement `wallet_get`
  int result = 0;
  pthread_mutex_lock(&wallet->mutex);
    resource_t* it = wallet->head;
    while(it) {
        if(strcmp(it->name, resource) == 0){
           result = it->count;
           break;
        }
        it = it->next;
    }
  pthread_mutex_unlock(&wallet->mutex);
  return result;
}

/**
 * Modifies the amount of a given `resource` in a given `wallet by `delta`.
 * - If `delta` is negative, this function MUST NOT RETURN until the resource can be satisfied.
 *   (Ths function MUST BLOCK until the wallet has enough resources to satisfy the request.)
 * - Returns the amount of resources in the wallet AFTER the change has been applied.
 */
int wallet_change_resource(wallet_t *wallet, const char *resource, int delta) {
    pthread_mutex_lock(&wallet->mutex); 
    resource_t *node = NULL;
    resource_t *current = wallet->head;
    while (current != NULL) {
        if (strcmp(current->name, resource) == 0) {
            node = current;
            break;
        }
        current = current->next;
    }
    if (!node) {
        node = malloc(sizeof(resource_t));
        node->name = strdup(resource); 
        node->count = 0; 
        node->next = wallet->head; 
        wallet->head = node;
    }
    while (node->count + delta < 0) {
        pthread_cond_wait(&wallet->condition, &wallet->mutex);
    }
    if (node) {
        node->count += delta;
    }
    pthread_cond_broadcast(&wallet->condition);
    pthread_mutex_unlock(&wallet->mutex); 
    return node->count;
}


/**
 * Destroys a wallet, freeing all associated memory.
 */
void wallet_destroy(wallet_t *wallet) {
  // Implement `wallet_destroy`
    resource_t *current = wallet->head;
    while (current != NULL) {
        resource_t *next_node = current->next;
        free(current->name); 
        free(current);
        current = next_node;
    }
    pthread_cond_destroy(&wallet->condition);
    pthread_mutex_destroy(&wallet->mutex);
}