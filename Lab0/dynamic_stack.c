/**
 *  Universidad Simón Bolívar
 *  EP5801 - Sistemas Embebidos I
 *  Laboratorio 0
 * 
 *  Parte 2.1. Pilas dinámicas
 *  
 *  Josmar Dominguez 16-10315
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/** == Constants == */
#define MAX_SIZE 10

enum Interruptions {
    TOGGLE_PIN_1    = 0x00,
    PRINT_PIN_1     = 0x01,
    ADD_COUNTER     = 0x02,
    PRINT_COUNTER   = 0x03,
    ITEMS_IN_STACK  = 0x05,
    FINISH_IF_FULL  = 0x06,
};

/** == Values == */
int pin_1 = 0;
size_t counter = 0;

/** == Structures == */
typedef struct Item {
    uint8_t value;
    struct Item* next;
} Item;

typedef struct Stack {
    uint8_t size;
    Item* top;
} Stack;

/** == Prototypes == */
Stack* createStack();

int push(Stack* stack, uint8_t value);

uint8_t pop(Stack* stack);

int isFull(Stack* stack);

int isEmpty(Stack* stack);

int processInterruption(Stack* stack, uint8_t interruption);

/** == Functions == */

// Function to create a new stack
Stack* createStack() {
    Stack* stack = (Stack *) malloc(sizeof(Stack));
    stack -> size = 0;
    stack -> top = NULL;

    return stack;
}

// Function to add an element to the stack
int push(Stack* stack, uint8_t value) {
    if (isFull(stack)) {
        return 1;
    }

    Item* newItem = (Item *) malloc(sizeof(Item));
    newItem -> value = value;
    newItem -> next = stack -> top;         // The new item points to the previous top
    stack -> top = newItem;                 // The new top is the new item
    stack -> size++;

    return 0;
}

// Function to remove an element from the stack
uint8_t pop(Stack* stack) {
    if (isEmpty(stack)) {
        return 0;
    }

    Item* oldTop = stack -> top;
    uint8_t value = oldTop -> value;
    stack -> top = oldTop -> next;          // The new top is the next item
    stack -> size--;
    free(oldTop);                           // Free the memory of the old top

    return value;
}

// Function to check if the stack is full
int isFull(Stack* stack) {
    return stack -> size == MAX_SIZE;
}

// Function to check if the stack is empty
int isEmpty(Stack* stack) {
    return !stack -> size;
}

// Function to process the interruptions
// Function to process the interruptions
int processInterruption(Stack* stack, uint8_t interruption) {
    switch (interruption) {
        case TOGGLE_PIN_1:
            printf("[0x00] Toggle pin 1\n");
            pin_1 = !pin_1;
            break;
        case PRINT_PIN_1:
            printf("[0x01] Print pin 1\n");
            printf("Pin 1: %d\n", pin_1);
            break;
        case ADD_COUNTER:
            printf("[0x02] Add counter\n");
            counter++;
            break;
        case PRINT_COUNTER:
            printf("[0x03] Print counter\n");
            printf("Counter: %ld\n", counter);
            break;
        case ITEMS_IN_STACK:
            printf("[0x05] Items in stack: %d\n", stack -> size);
            break;
        case FINISH_IF_FULL:
            if (isFull(stack)) {
                printf("[0x06] Stack is full\n");
                return 1;
            }
            printf("[0x06] Stack is not full\n");
            break;
        default:
            printf("[!!!] Invalid interruption\n");
            break;
    }

    return 0;
}

/** == Main == */
int main() {

    // Create the stack
    Stack* stack = createStack();

    // Interruptions
    uint8_t interruptions[10] = {
        0x00, 
        0x01,  
        0x05,
        0x03,
        0x02,  
        0x04,
        0x00,
        0x01,
        0x02,
        0x06
    };

    // Print the vector of interruptions
    printf("Interruptions: ");
    for (int i = 0; i < 10; i++) {
        printf("0x%02X ", interruptions[i]);
    }
    printf("\n");

    // Fill the stack
    for (int i = 0; i < 10; i++) {
        push(stack, interruptions[i]);
    }

    // Process the interruptions
    for (int i = 0; i < 10; i++) {
        printf("\n");
        processInterruption(stack, pop(stack));
    }

    return 0;
}

