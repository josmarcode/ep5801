/**
 *  Universidad Simón Bolívar
 *  EP5801 - Sistemas Embebidos I
 *  Laboratorio 0
 * 
 *  Parte 1. Uniones y estructuras
 *  
 *  Josmar Dominguez 16-10315
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/** == Structures == */
// Structure to store the processed data
typedef struct SensorData {
    uint8_t     battery_level;
    int16_t     ambient_temperature;
    int32_t     reserve_tank_level;
    uint32_t    main_tank_level;
    uint16_t    checksum;
} SensorData;

// Union to make the conversion between raw data and structured data
typedef union SensorUnion {
    uint8_t raw_data[12];                   // Input data
    struct {
        uint8_t     battery_level;          // Byte 0: Battery level
        uint8_t     ambient_temperature[2]; // Bytes 1-2: Ambient temperature
        uint8_t     reserve_tank_level[3];  // Byte 3-5: Reserve tank level
        uint8_t     main_tank_level[4];     // Byte 6-9: Main tank level
        uint8_t     checksum[2];            // Byte 10-11: Checksum
    } fields;
} SensorUnion;

/** == Prototypes == */
uint32_t mergeBytes(uint8_t* data, int size);
SensorData* processSensorData(uint8_t* data);


/** == Functions == */
// Function to merge an array of bytes into a single integer
uint32_t mergeBytes(uint8_t* data, int size) {
    uint32_t result = 0;

    for (int i = 0; i < size; i++) {
        result = result << 8 | data[i];
    }
    
    return result;
}

// Function to process the raw data and return the structured data
SensorData* processSensorData(uint8_t* data) {
    SensorUnion* sensorUnion = (SensorUnion *) malloc(sizeof(SensorUnion));
    SensorData* result = (SensorData *) malloc(sizeof(SensorData));

    // Copy the raw data into the union
    for (int i = 0; i < 12; i++) {
        sensorUnion -> raw_data[i] = data[i];
    }

    // Extract the data from the union
    result -> battery_level         = sensorUnion -> fields.battery_level;
    result -> ambient_temperature   = mergeBytes(sensorUnion -> fields.ambient_temperature, 2);
    result -> reserve_tank_level    = mergeBytes(sensorUnion -> fields.reserve_tank_level, 3);
    result -> main_tank_level       = mergeBytes(sensorUnion -> fields.main_tank_level, 4);
    result -> checksum              = mergeBytes(sensorUnion -> fields.checksum, 2);

    // Return the structured data
    return result;
}

/** == Main == */
int main() {

    // Test data from the sensor
    uint8_t sensorDataArray[12] = {
        0x62,                               // Nivel de batería (98)
        0xFF, 0xFE,                         // Temperatura ambiental (-2)
        0x00, 0x00, 0x69,                   // Nivel de tanque de reserva (105)
        0x00, 0x01, 0x81, 0x3C,             // Nivel de tanque principal (98620)
        0x01, 0x00                          // Checksum (256)
    };

    // Procesar los datos
    SensorData* sensorData = processSensorData(sensorDataArray);

    // Mostrar los resultados
    printf("Nivel de batería: %u\n", sensorData -> battery_level);
    printf("Temperatura ambiental: %d\n", sensorData -> ambient_temperature);
    printf("Nivel de tanque de reserva: %d\n", sensorData -> reserve_tank_level);
    printf("Nivel de tanque principal: %d\n", sensorData -> main_tank_level);
    printf("Checksum: %u\n", sensorData -> checksum);

    return 0;
}
