#include "queue_linear.h"
#include "queue_circular.h"
#include <stdio.h>

#define QUEUE_SIZE 5

// Define enumeration for pins
enum GPIO {
    CHIP_SELECT     = 0xA0,
    INT_1           = 0xA1,
    INT_2           = 0xA2,
    INT_3           = 0xA3,
    INT_4           = 0xA4
};

enum SPI {
    SDO = 0xE0,
    SDI = 0xE1,
    SCK = 0x32
};

enum REGISTERS {
    DEVICE          = 0xFF,
    GRAVITY_L       = 0x10,
    GRAVITY_H       = 0x11,
    INTERRUPT       = 0x20
};

// Define bitfields to registers
typedef union deviceDefinition {
    uint8_t value;
    struct {
        uint8_t gravity_size: 2;
        uint8_t extra_interrupts: 1;
        uint8_t device_state: 1;
        uint8_t reserved: 4;
    } fields;
} deviceDefinition;

typedef union gravityHigh {
    uint8_t value;
    struct {
        uint8_t higher_measurement;
    } fields;
} gravityHigh;

typedef union gravityLow {
    uint8_t value;
    struct {
        uint8_t lower_measurement: 6;
        uint8_t reserved : 2;
    } fields;
} gravityLow;

// [DEPRECATED]
typedef union gravityData {
    uint16_t value;
    struct {
        uint8_t higher_measurement;
        uint8_t lower_measurement;
        uint8_t reserved;
    } fields;
} gravityData;

typedef union interruptConfigure {
    uint8_t value;
    struct {
        uint8_t tap: 1;
        uint8_t gravity_change: 1;
        uint8_t inactivity: 1;
        uint8_t double_tap: 1;
        uint8_t reserved: 4;
    } fields;
} interruptConfigure;

// Define structure to items to enqueue as union to manage
// different types of data
typedef union data {
    uint8_t value;
    deviceDefinition device;
    gravityHigh gravityHigh;
    gravityLow gravityLow;
    interruptConfigure interrupt;
} data;


/** == First class passangers == */
typedef struct Item {
    uint8_t pin;                // Pin to use
    enum REGISTERS reg;         // Register to use
    data data;                  // Data to send/receive
} Item;

typedef struct Interruption {
    uint8_t pin;                    // Pin where the interruption was received
    char* description;              // Name of the interruption
} Interruption;


int get_screen_size(uint8_t data) {
    return 8 + (2 * data);
}

/** == Create functions == */
/**
 * Create a new interruption structure
 * based on the pin where the interruption was received
 * @param pin Pin where the interruption was received
 */
Interruption* create_interruption(uint8_t pin) {
    Interruption* interruption = (Interruption *) malloc(sizeof(Interruption));
    interruption -> pin = pin;
    
    char* description;
    switch (pin) {
        case INT_1:
            description = "Tap";
            break;
        case INT_2:
            description = "Gravity change";
            break;
        case INT_3:
            description = "Inactivity";
            break;
        case INT_4:
            description = "Double tap";
            break;
    }

    interruption -> description = description;

    return interruption;
}

/**
 *  Create a new item to store register data to
 *  send/receive from the device
 * @param pin Pin to use
 * @param value Value to send/receive
 * @param reg Register to use
 * @returns Item* Pointer to the new item
 */
Item* create_item(uint8_t pin, uint8_t value, enum REGISTERS reg) {
    // printf("Creating item with pin 0x%X and value 0x%X\n", pin, value);

    Item* item = (Item *) malloc(sizeof(Item));
    item -> pin = pin;
    item -> data.value = value;
    item -> reg = reg;

    return item;
}

/** == Print functions == */
// Print function for each type of register
// to be able to read the values
void print_device(union data* data) {
    // Extract the device register
    deviceDefinition* device = &data -> device;

    printf("Reading register 0xFF - Device Definitions\n");
    // printf("Device: %d\n", device -> value);
    printf("Gravity size: %d (%d bits)\n", device -> fields.gravity_size, get_screen_size(device -> fields.gravity_size));
    printf("Extra interrupts: %d\n", device -> fields.extra_interrupts);
    printf("Device state: %d\n", device -> fields.device_state);
}

void print_gravity_low(union data* data) {
    // Extract the gravity register
    gravityLow* gravity = &data -> gravityLow;

    printf("Reading register 0x10 - Gravity Data (lower byte)\n");
    // printf("Gravity: %d\n", gravity -> value);
    printf("Lower measurement: %d\n", gravity -> fields.lower_measurement);
}

void print_gravity_high(union data* data) {
    // Extract the gravity register
    gravityHigh* gravity = &data -> gravityHigh;

    printf("Reading register 0x11 - Gravity Data (higher byte)\n");
    // printf("Gravity: %d\n", gravity -> value);
    printf("Higher measurement: %d\n", gravity -> fields.higher_measurement);
}

void print_interrupt_config(union data* data) {
    // Extract the interrupt register
    interruptConfigure* interrupt = &data -> interrupt;

    printf("Reading register 0x20 - Interrupt Configuration\n");
    // printf("Interrupt: %d\n", interrupt -> value);
    printf("Tap: %d\n", interrupt -> fields.tap);
    printf("Gravity change: %d\n", interrupt -> fields.gravity_change);
    printf("Inactivity: %d\n", interrupt -> fields.inactivity);
    printf("Double tap: %d\n", interrupt -> fields.double_tap);
}

/**
 * Manager of the print functions, decide which
 * function to call based on the register
 * @param item Pointer to the item to print
 */
void print_item(Item* item) {
    printf("Pin: 0x%X\n", item -> pin);
    printf("Value: 0x%X\n", item -> data.value);

    char* action;
    
    // Pointer to function to print the data
    void (*print)(union data* data);

    switch (item -> reg) {
        case DEVICE:
            action = "Reading device configuration";
            print = print_device;
            break;
        case GRAVITY_L:
            action = "Reading gravity data (lower byte)";
            print = print_gravity_low;
            break;
        case GRAVITY_H:
            action = "Reading gravity data (higher byte)";
            print = print_gravity_high;
            break;
        case INTERRUPT:
            action = "Reading/writing interruption configuration";
            print = print_interrupt_config;
            break;
    }

    switch (item -> pin) {
        case SDO:
            printf("%s from the device by SDO\n", action);
            print(&item -> data);
            break;
        case SDI:
            printf("%s from the device by SDI\n", action);
            print(&item -> data);
            break;
    }
}

/**
 * Print the interruption received from the device
 * @param interruption Pointer to the interruption
 */
void print_interruption(Interruption* interruption) {
    printf("Reading interruption from pin 0x%X\n", interruption -> pin);
    printf("Interruption: %s\n", interruption -> description);
}

/** == Main == */
void main() {
    /** <- Simulation of SPI communication -> */
    printf("\t~> Simulation of SPI send/receive data <~\n");

    // Create a array of registers
    Item* registers[QUEUE_SIZE] = {
        create_item(SDI, 0x0A, DEVICE),
        create_item(SDO, 0x0A, INTERRUPT),
        create_item(SDI, 0x3F, INTERRUPT),
        create_item(SDI, 0x02, GRAVITY_L),
        create_item(SDO, 0xFF, GRAVITY_H)
    };

    // Create a linear queue to store the registers
    Queue* queue = linear_create_queue(QUEUE_SIZE);

    // Fill the queue with the items
    for (int i = 0; i < QUEUE_SIZE; i++) {
        linear_push(queue, registers[i]);
    }

    // Pop all the elements
    while (!linear_is_empty(queue)) {
        LinearNode* node = linear_pop(queue);
        Item* item = (Item *) node -> data;
        print_item(item);
        printf("\n");
    }

    printf("Is empty: %s\n\n\n", linear_is_empty(queue) ? "yes" : "no");

    free(queue);

    /** <- Simulation of receiving interruptions from device -> */
    printf("\t~> Simulation of receiving interruptions from device <~\n");

    // Create a array of interrupts
    uint8_t interrupts[QUEUE_SIZE] = {INT_1, INT_2, INT_3, INT_4, INT_1};

    // Create a circular queue to store the interrupts
    CircularQueue* circular_queue = create_circular_queue(QUEUE_SIZE);

    // Fill the queue with the items
    for (int i = 0; i < QUEUE_SIZE; i++) {
        Interruption* interruption = create_interruption(interrupts[i]);
        circular_push(circular_queue, interruption);
    }

    // Make a peek to view first element
    CircularNode* circular_node = circular_peek(circular_queue);

    // Pop all the elements
    while (!circular_is_empty(circular_queue)) {
        CircularNode* circular_node = circular_pop(circular_queue);
        Interruption* interrupt = (Interruption *) circular_node -> data;
        print_interruption(interrupt);
        printf("\n");
    }

    printf("Is empty: %s\n", circular_is_empty(circular_queue) ? "yes" : "no");

}