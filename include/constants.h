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
#define JOYSTICK_READ_INTERVAL 1000

// Period of status LED blink when data is received in milliseconds
#define STATUS_LED_BLINK_PERIOD 10

// IO pins
#define STATUS_LED   2
#define POWER_BUTTON 16
#define BOARD_POWER  27

// LEDs
#define LED_BUTTON_A 14
#define LED_BUTTON_B 12
#define LED_BUTTON_C 13

// Joystick analog inputs - only ADC1 pins, cause ADC2 is used by Wi-Fi
#define BOOM_JOYSTICK        32
#define STICK_JOYSTICK       35
#define BUCKET_JOYSTICK      33
#define SWING_JOYSTICK       34
#define TRACK_LEFT_JOYSTICK  36
#define TRACK_RIGHT_JOYSTICK 39

#endif // _CONSTANTS_H