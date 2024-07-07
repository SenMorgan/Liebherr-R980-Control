/**
 * @file lever_control.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include "lever_control.h"

#include <Arduino.h>
#include "driver/adc.h"

Lever::Lever(uint8_t _pin,
             uint16_t _minAdcVal,
             uint16_t _maxAdcVal,
             bool _invert,
             uint16_t _deadZone,
             bool _exponentialSmoothing)
{
    pin = _pin;
    minAdcVal = _minAdcVal;
    maxAdcVal = _maxAdcVal;
    invert = _invert;
    deadZone = _deadZone;
    exponentialSmoothing = _exponentialSmoothing;
    minOutput = -1023;
    maxOutput = +1023;
    pos = 0;
    lastPos = -999; // Set to an invalid value to force the first update
    rawValue = 0;
    updateInterval = 10;
    lastUpdateTime = 0;

    pinMode(pin, INPUT);
    analogReadResolution(10);       // Set ADC resolution to 10 bits (0-1023 range)
    analogSetAttenuation(ADC_11db); // Set ADC attenuation to 11dB (0-3.6V range)
}

void Lever::calibrate()
{
    zeroPos = analogRead(pin);

    // Fill the readings buffer with the current value
    for (uint8_t i = 0; i < numReadings; i++)
    {
        readings[i] = zeroPos;
        total += zeroPos;
    }
}

bool Lever::update()
{
    uint32_t currentTime = millis();

    // Check if the update interval has passed
    if (currentTime - lastUpdateTime > updateInterval)
    {
        lastUpdateTime = currentTime;     // Update the last update time
        pos = readAndFilter();            // Get the new position
        bool hasChanged = lastPos != pos; // Check if the position has changed
        lastPos = pos;                    // Update the last position

        return hasChanged; // Return true if the position has changed, false otherwise
    }

    return false;
}

int16_t Lever::position() const
{
    return pos;
}

uint16_t Lever::value() const
{
    return rawValue;
}

String Lever::printDebug()
{
    return "Pos: " + String(pos) + " Raw: " + String(rawValue) + " Zero: " + String(zeroPos);
}

int16_t Lever::readAndFilter()
{
    // Subtract the last reading
    total = total - readings[readIndex];
    // Read from the sensor
    rawValue = analogRead(pin);
    readings[readIndex] = rawValue;
    // Add the reading to the total
    total = total + readings[readIndex];
    // Advance to the next position in the array
    readIndex = (readIndex + 1) % numReadings;
    // Calculate the average
    calcRes = total / numReadings;

    // Adjust for center position and dead-zone
    if (abs(calcRes - zeroPos) < deadZone)
    {
        calcRes = 0; // Within dead-zone
    }
    else
    {
        // Map the value considering the dead-zone
        if (calcRes < zeroPos)
            calcRes = map(calcRes, minAdcVal, zeroPos - deadZone, minOutput, 0);
        else
            calcRes = map(calcRes, zeroPos + deadZone, maxAdcVal, 0, maxOutput);

        // Constrain the value to the output range
        calcRes = constrain(calcRes, minOutput, maxOutput);

        // Use exponential function to smooth out the output if needed
        if (exponentialSmoothing)
            calcRes = pow(calcRes, 3) / pow(maxOutput, 2);

        // Invert the value if needed
        if (invert)
            calcRes = -calcRes;
    }

    // Return result
    return calcRes;
}