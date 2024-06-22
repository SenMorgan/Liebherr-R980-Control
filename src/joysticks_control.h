/**
 * @file joysticks_control.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef JOYSTICKS_CONTROL_H
#define JOYSTICKS_CONTROL_H

#include <Arduino.h>

class Joystick
{
private:
    uint8_t pin;
    uint16_t minAdcVal;      // Minimum expected analog value
    uint16_t maxAdcVal;      // Maximum expected analog value
    uint16_t zeroPos;        // Center position of the joystick
    uint16_t deadZone;       // Dead zone value
    bool invert;             // Invert the joystick value
    int16_t pos;             // Current joystick calculated position
    int16_t rawValue;        // Current joystick raw value
    int16_t minOutput;       // Minimum output value
    int16_t maxOutput;       // Maximum output value
    uint16_t updateInterval; // Period between readings in milliseconds
    uint32_t lastUpdateTime; // Last reading time in milliseconds

    /**
     * @brief Reads the joystick input and filters the readings using a moving average algorithm and exponential smoothing.
     * @return The filtered joystick value.
     */
    uint16_t readAndFilter();

public:
    /**
     * @brief Construct a new Joystick object.
     */
    Joystick(uint8_t _pin,
             uint16_t _minAdcVal = 0,
             uint16_t _maxAdcVal = 1023,
             bool _invert = false,
             uint16_t _deadZone = 30);

    /**
     * @brief Calibrates the joystick by reading the center position.
     */
    void calibrate();

    /**
     * @brief Updates the joystick value by reading and filtering the input.
     * @return True if reading was performed, false otherwise.
     */
    bool update();

    /**
     * @brief Returns the calculated, mapped and filtered joystick position.
     * @return The current joystick position.
     */
    int position() const;

    /**
     * @brief Returns last raw ADC value of the joystick.
     * @return The raw value of the joystick.
     */
    uint16_t value() const;

    /**
     * @brief Returns a string with the joystick debug information.
     * @return A string with the joystick debug information.
     */
    String printDebug();
};

#endif // JOYSTICKS_CONTROL_H