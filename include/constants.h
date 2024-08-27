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

// Minimum and maximum interval between sending data in milliseconds
#define SEND_DATA_MIN_INTERVAL 25
#define SEND_DATA_MAX_INTERVAL 10000

// Period of user inactivity to read the battery voltage in milliseconds (will disable Wi-Fi for a few seconds)
#define INACTIVITY_PERIOD_FOR_BATTERY_READ 5000
// Period of inactivity to power off the board in milliseconds
#define INACTIVITY_PERIOD_FOR_POWER_OFF    10 * 60 * 1000 // 10 minutes
// Period between battery voltage readings in milliseconds
#define BATTERY_READ_INTERVAL              5 * 60 * 1000 // 5 minutes

// Number of levers
#define LEVERS_COUNT 6

// Number of buttons
#define BUTTONS_COUNT 3

// Communication pins
#define I2C_SDA GPIO_NUM_21
#define I2C_SCL GPIO_NUM_22

// Output pins
#define BOARD_POWER GPIO_NUM_14

// LEDs
#define STATUS_LED   GPIO_NUM_2
#define LED_BUTTON_A GPIO_NUM_27
#define LED_BUTTON_B GPIO_NUM_12
#define LED_BUTTON_C GPIO_NUM_13

// Buttons
#define POWER_BUTTON             GPIO_NUM_15
#define MAIN_LIGHTS_BUTTON       GPIO_NUM_17
#define CENTER_SWING_BUTTON      GPIO_NUM_18
#define BEACON_LIGHT_MODE_BUTTON GPIO_NUM_19
#define OPT_2_BUTTON             GPIO_NUM_23
#define A_BUTTON                 GPIO_NUM_25
#define B_BUTTON                 GPIO_NUM_26
#define C_BUTTON                 GPIO_NUM_16
#define SCAN_BUTTON              GPIO_NUM_5

// Lever analog inputs - only ADC1 pins, cause ADC2 is used by Wi-Fi
#define BOOM_LEVER         GPIO_NUM_32
#define STICK_LEVER        GPIO_NUM_35
#define BUCKET_LEVER       GPIO_NUM_33
#define SWING_LEVER        GPIO_NUM_34
#define LEFT_TRAVEL_LEVER  GPIO_NUM_39
#define RIGHT_TRAVEL_LEVER GPIO_NUM_36

// Battery voltage divider (ADC2 - Wi-Fi must be disabled during readings)
#define BATTERY_VOLTAGE_PIN GPIO_NUM_4

#endif // _CONSTANTS_H