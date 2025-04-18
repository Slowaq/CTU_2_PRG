// #ifndef __QUEUE_H__
// #define __QUEUE_H__
// #endif /* __QUEUE_H__ */

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// int main(int argc, char *argv[])
// {
//    return 0;
// }

/* creates a new queue with a given size */
queue_t* create_queue(int capacity)
{
    if (capacity <= 0) return NULL;
    queue_t *temp = malloc(sizeof(queue_t));
    if (temp == NULL) {
        free(temp);
        return NULL;
    }
    void **temp_arr = malloc(capacity * sizeof(void *));
        if (temp_arr == NULL){
            free(temp_arr);
        return NULL;
        }
    queue_t *queue = temp;

    queue->array = temp_arr;
    queue->capacity = capacity;
    queue->head = 0;
    queue->tail = 0;
    queue->occupancy = 0;

    return queue;
}

/* deletes the queue and all allocated memory */
void delete_queue(queue_t *queue)
{
   if (queue){
        free(queue->array);
        free(queue);
   }
}

/*
 * inserts a reference to the element into the queue
 * returns: true on success; false otherwise
 */
bool push_to_queue(queue_t *queue, void *data)
{
    if (queue->occupancy == (queue->capacity) ){
        resize_queue(queue, queue->capacity * 2);
    }

    queue->array[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->occupancy++;

    return true;
}

/*
 * gets the first element from the queue and removes it from the queue
 * returns: the first element on success; NULL otherwise
 */
void* pop_from_queue(queue_t *queue)
{
    if (queue->occupancy == 0) return NULL;

    if (queue->occupancy * 3<= queue->capacity){
        resize_queue(queue, queue->capacity / 3);
    }

    void *first = queue->array[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    queue->occupancy--;

    return first;
}

/*
 * gets idx-th element from the queue, i.e., it returns the element that 
 * will be popped after idx calls of the pop_from_queue()
 * returns: the idx-th element on success; NULL otherwise
 */
void* get_from_queue(queue_t *queue, int idx)
{
    if (!queue || idx < 0 || idx >= queue->occupancy) return NULL;

    int pos = (queue->head + idx) % queue->capacity;
    return queue->array[pos];
}

/* gets number of stored elements */
int get_queue_size(queue_t *queue)
{
    return queue ? queue->occupancy : 0;
}

bool resize_queue(queue_t *queue, int new_capacity)
{
    if (!queue) return false;

    void **temp = malloc(new_capacity * sizeof(void *));
    if (temp == NULL){
        free(temp);
        return false;
    }
    for (int i = 0; i < queue->occupancy; ++i){
        temp[i] = queue->array[(queue->head + i) % queue->capacity];
    }
    free(queue->array);
    queue->array = temp;
    queue->capacity = new_capacity;
    queue->head = 0;
    queue->tail = queue->occupancy;

    return true;
    }

