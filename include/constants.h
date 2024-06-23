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

// Period between sending data in milliseconds
#define SEND_DATA_INTERVAL 50

// Period of status LED blink when data is received in milliseconds
#define STATUS_LED_BLINK_PERIOD 10

// Period of user inactivity to read the battery voltage in milliseconds (will disable Wi-Fi for a few seconds)
#define INACTIVITY_PERIOD_FOR_BATTERY_READ 5000
// Period between battery voltage readings in milliseconds
#define BATTERY_READ_INTERVAL 60000

// Communication pins
#define I2C_SDA 21
#define I2C_SCL 22

// Output pins
#define BOARD_POWER 14

// LEDs
#define STATUS_LED   2
#define LED_BUTTON_A 27
#define LED_BUTTON_B 12
#define LED_BUTTON_C 13

// Buttons
#define POWER_BUTTON        16
#define MAIN_LIGHTS_BUTTON  17
#define CENTER_SWING_BUTTON 18
#define SCAN_BUTTON         5
#define OPT_1_BUTTON        19
#define OPT_2_BUTTON        23
#define A_BUTTON            25
#define B_BUTTON            26
#define C_BUTTON            15

// Lever analog inputs - only ADC1 pins, cause ADC2 is used by Wi-Fi
#define BOOM_LEVER         32
#define STICK_LEVER        35
#define BUCKET_LEVER       33
#define SWING_LEVER        34
#define LEFT_TRAVEL_LEVER  36
#define RIGHT_TRAVEL_LEVER 39

// Battery voltage divider (ADC2 - Wi-Fi must be disabled during readings)
#define BATTERY_VOLTAGE_PIN 4

#endif // _CONSTANTS_H