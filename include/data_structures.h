/**
 * @file data_structures.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdint.h>

// The structure type of the data that will be sent over ESP-NOW from the Controller to the Excavator
typedef struct controller_data_struct
{
    int boom;       // Position for the boom joystick
    int dipper;     // Position for the dipper joystick
    int bucket;     // Position for the bucket joystick
    int swing;      // Position for the swing joystick
    int trackLeft;  // Position for the left track joystick
    int trackRight; // Position for the right track joystick
} controller_data_struct;

// The structure type of the data that will be sent over ESP-NOW from the Excavator to the Controller
typedef struct excavator_data_struct
{
    uint16_t uptime;  // Uptime of the Excavator in seconds
    uint16_t battery; // Battery level of the Excavator
    float cpuTemp;    // CPU temperature of the Excavator
} excavator_data_struct;

#endif // DATA_STRUCTURES_H