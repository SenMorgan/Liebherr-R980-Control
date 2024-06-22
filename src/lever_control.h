/**
 * @file lever_control.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef LEVER_CONTROL_H
#define LEVER_CONTROL_H

#include <Arduino.h>

class Lever
{
private:
    uint8_t pin;
    uint16_t minAdcVal;      // Minimum expected analog value
    uint16_t maxAdcVal;      // Maximum expected analog value
    uint16_t zeroPos;        // Center position of the lever
    uint16_t deadZone;       // Dead zone value
    bool invert;             // Invert the lever value
    int16_t pos;             // Current lever calculated position
    int16_t rawValue;        // Current lever raw value
    int16_t minOutput;       // Minimum output value
    int16_t maxOutput;       // Maximum output value
    uint16_t updateInterval; // Period between readings in milliseconds
    uint32_t lastUpdateTime; // Last reading time in milliseconds

    /**
     * @brief Reads the lever input and filters the readings using a moving average algorithm and exponential smoothing.
     * @return The filtered lever value.
     */
    uint16_t readAndFilter();

public:
    /**
     * @brief Construct a new Lever object.
     */
    Lever(uint8_t _pin,
          uint16_t _minAdcVal = 0,
          uint16_t _maxAdcVal = 1023,
          bool _invert = false,
          uint16_t _deadZone = 30);

    /**
     * @brief Calibrates the lever by reading the center position.
     */
    void calibrate();

    /**
     * @brief Updates the lever value by reading and filtering the input.
     * @return True if reading was performed, false otherwise.
     */
    bool update();

    /**
     * @brief Returns the calculated, mapped and filtered lever position.
     * @return The current lever position.
     */
    int position() const;

    /**
     * @brief Returns last raw ADC value of the lever.
     * @return The raw value of the lever.
     */
    uint16_t value() const;

    /**
     * @brief Returns a string with the lever debug information.
     * @return A string with the lever debug information.
     */
    String printDebug();
};

#endif // LEVER_CONTROL_H