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
#include <map>

// Map to keep track of timers by GPIO pin number
static std::map<gpio_num_t, esp_timer_handle_t> timers;

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
    // Check if a timer for this pin already exists
    auto ledTimerIterator = timers.find(ledPin);
    if (ledTimerIterator != timers.end())
    {
        // Timer exists, restart it with the new duration
        esp_timer_stop(ledTimerIterator->second);
        esp_timer_start_once(ledTimerIterator->second, duration * 1000);
    }
    else
    {
        // No timer for this pin, create a new one
        // Allocate memory for the gpio_num_t pointer to pass as an argument to the timer callback
        gpio_num_t *ledPinPtr = new gpio_num_t(ledPin);

        const esp_timer_create_args_t timerArgs = {
            .callback = &timerCallback,
            .arg = ledPinPtr, // Pass the ledPin pointer as an argument
            .name = "ledOffTimer"};

        esp_timer_handle_t timerHandle;
        esp_timer_create(&timerArgs, &timerHandle);

        // Add the new timer to the map
        timers[ledPin] = timerHandle;

        // Start the timer. Duration is in microseconds
        esp_timer_start_once(timerHandle, duration * 1000);
    }

    // Turn on the LED
    digitalWrite(ledPin, HIGH);
}