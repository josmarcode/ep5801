#ifndef QUEUE_CIRCULAR_H
#define QUEUE_CIRCULAR_H

#include <stdint.h>
#include <stdlib.h>

// CircularNode structure
typedef struct CircularNode {
    void* data;         // Pointer to void (generic data)
    struct CircularNode* next;  // Pointer to the next node
} CircularNode;

// Queue structure
typedef struct CircularQueue {
    CircularNode* front;        // Pointer to the front of the queue
    CircularNode* rear;         // Pointer to the rear of the queue
    size_t capacity;    // Max capacity of the queue
    size_t size;        // Current size of the queue
} CircularQueue;

// Create a new queue
CircularQueue* create_circular_queue(size_t capacity);

// Push data to the queue
int circular_push(CircularQueue* queue, void* data);

// Pop data from the queue
CircularNode* circular_pop(CircularQueue* queue);

// Check last element of the queue without removing it
CircularNode* circular_peek(CircularQueue* queue);

// Check if the queue is empty
int circular_is_empty(CircularQueue* queue);

// Check if the queue is full
int circular_is_full(CircularQueue* queue);


#endif