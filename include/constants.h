/**
 * @file constants.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

// Period between joystick readings in milliseconds
#define JOYSTICK_READ_PERIOD 1000

// Period of status LED blink when data is received in milliseconds
#define STATUS_LED_BLINK_PERIOD 10

// IO pins
#define STATUS_LED          2

// Joystick analog inputs
#define BOOM_JOYSTICK        36
#define DIPPER_JOYSTICK      39
#define BUCKET_JOYSTICK      34
#define SWING_JOYSTICK       35
#define TRACK_LEFT_JOYSTICK  32
#define TRACK_RIGHT_JOYSTICK 33

#endif // _CONSTANTS_H