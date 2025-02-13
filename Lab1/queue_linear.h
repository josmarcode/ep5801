#ifndef QUEUE_LINEAR_H
#define QUEUE_LINEAR_H

#include <stdint.h>
#include <stdlib.h>

// LinearNode structure
typedef struct LinearNode {
    void* data;         // Pointer to the data
    struct LinearNode* next;  // Pointer to the next node
} LinearNode;

// Queue structure
typedef struct Queue {
    LinearNode* front;        // Pointer to the front of the queue
    size_t capacity;    // Max capacity of the queue
    size_t size;        // Current size of the queue
} Queue;

// Create a new queue
Queue* linear_create_queue(size_t capacity);

// Push data to the queue
int linear_push(Queue* queue, void* data);

// Pop data from the queue
LinearNode* linear_pop(Queue* queue);

// Check last element of the queue without removing it
LinearNode* linear_peek(Queue* queue);

// Check if the queue is empty
int linear_is_empty(Queue* queue);

// Check if the queue is full
int linear_is_full(Queue* queue);


#endif