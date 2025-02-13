#include "queue_circular.h"
#include <stdio.h>

/**
 * Create a new queue
 * @param capacity Max capacity of the queue
 * @returns Queue* Pointer to the new queue
 */
CircularQueue* create_circular_queue(size_t capacity) {
    CircularQueue* queue = (CircularQueue *) malloc(sizeof(CircularQueue));
    queue -> front = NULL;
    queue -> rear = NULL;
    queue -> capacity = capacity;
    queue -> size = 0;

    return queue;
}

/**
 * Push data to the queue
 * @param queue Pointer to the queue
 * @param data Pointer to the data
 * @returns int 0 if the operation was successful, 1 if the queue is full
 */
int circular_push(CircularQueue* queue, void* data) {
    if (circular_is_full(queue)) {
        return 1;
    }

    // Create a new node with data
    CircularNode* node = (CircularNode *) malloc(sizeof(CircularNode));
    node -> data = data;
    node -> next = NULL;

    // If the queue is empty, the new node is the front and rear
    if (circular_is_empty(queue)) {
        queue -> front = node;
        queue -> rear = node;
        queue -> size++;
        return 0;
    }

    // Add the node to the end
    queue -> rear -> next = node;               // Add the new rear
    queue -> rear = node;                       // Update the rear in the queue
    queue -> rear -> next = queue -> front;     // Point to the front
    queue -> size++;

    return 0;
}

/**
 * Pop data from the queue
 * @param queue Pointer to the queue
 * @returns void* Pointer to the data
 */
CircularNode* circular_pop(CircularQueue* queue) {
    if (circular_is_empty(queue)) {
        return NULL;
    }

    // Get the data from the front
    CircularNode* node = queue -> front;

    // Update the front
    queue -> front = node -> next;
    queue -> rear -> next = queue -> front;     // Point to the new front
    queue -> size--;

    return node;
}

/**
 * Check last element of the queue without removing it
 * @param queue Pointer to the queue
 * @returns void* Pointer to the data
 */
CircularNode* circular_peek(CircularQueue* queue) {
    return queue -> front;
}

/**
 * Check if the queue is empty
 * @param queue Pointer to the queue
 * @returns int 1 if the queue is empty, 0 otherwise
 */
int circular_is_empty(CircularQueue* queue) {
    return queue -> size == 0;
}

/**
 * Check if the queue is full
 * @param queue Pointer to the queue
 * @returns int 1 if the queue is full, 0 otherwise
 */
int circular_is_full(CircularQueue* queue) {
    return queue -> size == queue -> capacity;
}