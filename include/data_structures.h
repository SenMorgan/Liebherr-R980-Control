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
    int boom;        // Position of the boom lever
    int dipper;      // Position of the dipper lever
    int bucket;      // Position of the bucket lever
    int swing;       // Position of the swing lever
    int travelLeft;  // Position of the left travel lever
    int travelRight; // Position of the right travel lever
} controller_data_struct;

// The structure type of the data that will be sent over ESP-NOW from the Excavator to the Controller
typedef struct excavator_data_struct
{
    uint16_t uptime;  // Excavator controller uptime
    uint16_t battery; // Excavator battery voltage
    float cpuTemp;    // Excavator controller temperature
} excavator_data_struct;

#endif // DATA_STRUCTURES_H