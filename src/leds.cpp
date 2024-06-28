/**
 * @file leds.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-28
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include "leds.h"
#include "esp_timer.h"

// Callback function to turn off the LED
/**
 * @brief Callback function for the timer.
 *
 * This function is called when the timer expires. It turns off the LED
 * associated with the given GPIO pin.
 *
 * @param arg Pointer to the GPIO pin number (gpio_num_t) associated with the LED.
 */
void IRAM_ATTR timerCallback(void *arg)
{
    // Cast arg back to gpio_num_t
    gpio_num_t ledPin = *(gpio_num_t *)arg;
    // Turn off the LED
    digitalWrite(ledPin, LOW);
}

/**
 * @brief Blinks an LED for a specified duration.
 *
 * This function turns on an LED for a specified duration and then turns it off.
 * It uses a single-shot timer to control the duration of the LED blink.
 *
 * @param ledPin The GPIO pin number of the LED.
 * @param duration The duration of the LED blink in milliseconds.
 */
void blinkWithLed(gpio_num_t ledPin, uint32_t duration)
{
    // Allocate memory for ledPin to pass to the callback
    gpio_num_t *ledPinPtr = new gpio_num_t(ledPin);

    // Create a single-shot timer
    const esp_timer_create_args_t timerArgs = {
        .callback = &timerCallback,
        .arg = ledPinPtr, // Pass the ledPin pointer as an argument
        .name = "ledOffTimer"};

    esp_timer_handle_t timerHandle;
    esp_timer_create(&timerArgs, &timerHandle);

    // Turn on the LED
    digitalWrite(ledPin, HIGH);

    // Start the timer. Duration is in microseconds
    esp_timer_start_once(timerHandle, duration * 1000);
}