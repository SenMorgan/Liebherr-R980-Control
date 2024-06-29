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
    uint16_t minAdcVal;        // Minimum expected analog value
    uint16_t maxAdcVal;        // Maximum expected analog value
    uint16_t zeroPos;          // Center position of the lever
    uint16_t deadZone;         // Dead zone value
    bool invert;               // Invert the lever value
    bool exponentialSmoothing; // Use exponential smoothing
    int16_t pos;               // Current lever calculated position
    int16_t lastPos;           // Last lever calculated position
    uint16_t rawValue;         // Current lever raw value
    int16_t minOutput;         // Minimum output value
    int16_t maxOutput;         // Maximum output value
    uint16_t updateInterval;   // Period between readings in milliseconds
    uint32_t lastUpdateTime;   // Last reading time in milliseconds

    uint8_t numReadings = 10; // Number of readings to average
    uint16_t readings[10];    // Readings buffer (size must be the same as numReadings)
    uint8_t readIndex = 0;    // Index of the current reading
    uint32_t total = 0;       // Running total
    int16_t calcRes = 0;      // Calculated result

    /**
     * @brief Reads the lever input and filters the readings using a moving average algorithm and exponential smoothing.
     * @return The filtered lever value.
     */
    int16_t readAndFilter();

public:
    /**
     * @brief Construct a new Lever object.
     */
    Lever(uint8_t _pin,
          uint16_t _minAdcVal = 0,
          uint16_t _maxAdcVal = 1023,
          bool _invert = false,
          uint16_t _deadZone = 40,
          bool _exponentialSmoothing = false);

    /**
     * @brief Calibrates the lever by reading the center position.
     */
    void calibrate();

    /**
     * @brief Updates the lever value by reading and filtering the input.
     * @return True if position has changed, false otherwise.
     */
    bool update();

    /**
     * @brief Returns the calculated, mapped and filtered lever position.
     * @return The current lever position.
     */
    int16_t position() const;

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