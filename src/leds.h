/**
 * @file leds.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-28
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

void blinkWithLed(gpio_num_t ledPin, uint32_t duration = 10);

#endif // LEDS_H