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

// Period between lever readings in milliseconds
#define LEVER_READ_INTERVAL 1000

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

// Lever analog inputs - only ADC1 pins, cause ADC2 is used by Wi-Fi
#define BOOM_LEVER         32
#define STICK_LEVER        35
#define BUCKET_LEVER       33
#define SWING_LEVER        34
#define LEFT_TRAVEL_LEVER  36
#define RIGHT_TRAVEL_LEVER 39

#endif // _CONSTANTS_H