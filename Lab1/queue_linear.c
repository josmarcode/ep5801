#include "queue_linear.h"
#include <stdio.h>

/**
 * Create a new queue
 * @param capacity Max capacity of the queue
 * @returns Queue* Pointer to the new queue
 */
Queue* linear_create_queue(size_t capacity) {
    Queue* queue = (Queue *) malloc(sizeof(Queue));
    queue -> front = NULL;
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
int linear_push(Queue* queue, void* data) {
    if (linear_is_full(queue)) {
        return 1;
    }

    // Create a new node with data
    LinearNode* node = (LinearNode *) malloc(sizeof(LinearNode));
    node -> data = data;
    node -> next = NULL;

    // If the queue is empty, the new node is the front
    if (linear_is_empty(queue)) {
        queue -> front = node;
        queue -> size++;
        return 0;
    }

    // Find the last node
    LinearNode* last = queue -> front;
    while (last -> next != NULL) {
        last = last -> next;
    }

    // Add the node to the end
    last -> next = node;
    queue -> size++;

    return 0;
}

/**
 * Pop data from the queue
 * @param queue Pointer to the queue
 * @returns void* Pointer to the data
 */
LinearNode* linear_pop(Queue* queue) {
    if (linear_is_empty(queue)) {
        return NULL;
    }

    // Get the data from the front
    LinearNode* node = queue -> front;

    // Update the front
    queue -> front = node -> next;
    queue -> size--;

    return node;
}

/**
 * Check last element of the queue without removing it
 * @param queue Pointer to the queue
 * @returns void* Pointer to the data
 */
LinearNode* linear_peek(Queue* queue) {
    return queue -> front;
}

/**
 * Check if the queue is empty
 * @param queue Pointer to the queue
 * @returns int 1 if the queue is empty, 0 otherwise
 */
int linear_is_empty(Queue* queue) {
    return queue -> size == 0;
}

/**
 * Check if the queue is full
 * @param queue Pointer to the queue
 * @returns int 1 if the queue is full, 0 otherwise
 */
int linear_is_full(Queue* queue) {
    return queue -> size == queue -> capacity;
}