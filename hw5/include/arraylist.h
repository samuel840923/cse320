#ifndef ARRAYLIST_H
#define ARRAYLIST_H
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include "const.h"

/*
    _  _____ _____ _   _     ____  _     _____    ____  _____    _    ____
   / \|_   _|_   _| \ | |   |  _ \| |   |__  /   |  _ \| ____|  / \  |  _ \
  / _ \ | |   | | |  \| |   | |_) | |     / /    | |_) |  _|   / _ \ | | | |
 / ___ \| |   | | | |\  |   |  __/| |___ / /_    |  _ <| |___ / ___ \| |_| |
/_/   \_\_|   |_| |_| \_|   |_|   |_____/____|   |_| \_\_____/_/   \_\____/
*/

typedef struct{
    /* BEGIN: DO NOT MODIFY THE FIELDS BETWEEN THESE COMMENTS */
    size_t capacity;
    size_t length;
    size_t item_size;
    void* base;
    /* END: .. add locks, other fields BELOW THIS COMMENT if needed .. */

}arraylist_t;

arraylist_t *new_al(size_t item_size);

size_t insert_al(arraylist_t *self, void* data);

void *get_data_al(arraylist_t *self, void *data);
void *get_index_al(arraylist_t *self, size_t index);

void *remove_data_al(arraylist_t *self, void *data);
void *remove_index_al(arraylist_t *self, size_t index);

void delete_al(arraylist_t *self, void (*free_item_func)(void*));
int getindex(arraylist_t *self, void *data);
int shiftleft (arraylist_t *self, int index);

#endif
