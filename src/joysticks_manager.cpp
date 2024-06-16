/**
 * @file joysticks_manager.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include <Arduino.h>
#include "joysticks_manager.h"
#include "constants.h"

void setJoysticksPins()
{
    // Set the joysticks pins as inputs
    pinMode(BOOM_JOYSTICK, INPUT);
    pinMode(DIPPER_JOYSTICK, INPUT);
    pinMode(BUCKET_JOYSTICK, INPUT);
    pinMode(SWING_JOYSTICK, INPUT);
    pinMode(TRACK_LEFT_JOYSTICK, INPUT);
    pinMode(TRACK_RIGHT_JOYSTICK, INPUT);
}

void readJoysticksPositions(controller_data_struct &data)
{
    // Read the analog values of the joysticks
    data.boom = analogRead(BOOM_JOYSTICK);
    data.dipper = analogRead(DIPPER_JOYSTICK);
    data.bucket = analogRead(BUCKET_JOYSTICK);
    data.swing = analogRead(SWING_JOYSTICK);
    data.trackLeft = analogRead(TRACK_LEFT_JOYSTICK);
    data.trackRight = analogRead(TRACK_RIGHT_JOYSTICK);
}