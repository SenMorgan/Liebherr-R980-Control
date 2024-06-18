/**
 * @file joysticks_control.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef JOYSTICKS_MANAGER_H
#define JOYSTICKS_MANAGER_H

#include "data_structures.h"

void setJoysticksPins();
void readJoysticksPositions(controller_data_struct &data);

#endif // JOYSTICKS_MANAGER_H