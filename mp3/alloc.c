/**
 * Malloc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct metadata_t {
  // Add any elements you need to store the PNG here:
  int status;
  int size;
  void* next;

}metadata_t;

void* start = NULL;
metadata_t* head = NULL;

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) { 
    size_t total_size = num * size;
    void* ptr = malloc(total_size);
    if (!ptr) {  
        return NULL;
    }
    // Initialize memory to 0
    memset(ptr, 0, total_size);
    return ptr;
}


/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
    if(!start) {start = sbrk(0);}
    metadata_t * it = head;
    metadata_t * prev = NULL;
    while (it) {
        if(it->size >= size + sizeof(metadata_t)) {
            int remaining_size = it->size - size - sizeof(metadata_t);
            if(remaining_size > 0) {
                it->size = size;
                it->status = 1;
                metadata_t* new_block = (char*)it + sizeof(metadata_t) + size;
                new_block->status = 0; 
                new_block->size = remaining_size;
                new_block->next = it->next;
                it->next = new_block;
            }
            if(prev) {
                prev->next = it->next;
            } else {
                head = it->next; 
            }
            return (char*)it + sizeof(metadata_t); 
        }
        prev = it;
        it = it->next;
    }
    metadata_t* new_data = sbrk(sizeof(metadata_t));
    new_data->size = size;
    new_data->status = 1;
    new_data->next = NULL;  // It's not part of the free list yet
    void* result = sbrk(size);
    return result;
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void* ptr) {
    if (!ptr) {
    return; 
    }
    metadata_t* block_to_free = ptr - sizeof(metadata_t);
    block_to_free->status = 0;
    if (!head) {
        head = block_to_free;
        block_to_free->next = NULL;
        return;
    }
    metadata_t *current = head;
    metadata_t *prev = NULL;
    while (current && (block_to_free > current)) {
        prev = current;
        current = current->next;
    }
    if (prev) {
        prev->next = block_to_free;
    } else {
        head = block_to_free; 
    }
    block_to_free->next = current;
    metadata_t* next_block = ((char*)block_to_free + sizeof(metadata_t) + block_to_free->size);
    if (block_to_free->next == next_block && next_block->status == 0) {
        block_to_free->size += next_block->size + sizeof(metadata_t);
        block_to_free->next = next_block->next;
    }
    if (prev && prev->status == 0) {
        prev->size += block_to_free->size + sizeof(metadata_t);
        prev->next = block_to_free->next;
    }
}



/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
  if (!ptr) {
        return malloc(size);
  }
  if (size == 0) {
        free(ptr);
        return NULL;
  }
  metadata_t* location = ptr - sizeof(metadata_t);
  if (location->size > size) {
        location->size = size;
        return ptr;
  } else if (location->size < size) {
        void* new_ptr = malloc(size);
        memcpy(new_ptr, ptr, size);
        free(ptr);
        return new_ptr;
  } else {
        return ptr;
  }
    return NULL;
}
