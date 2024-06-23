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
             uint16_t _deadZone)
{
    pin = _pin;
    minAdcVal = _minAdcVal;
    maxAdcVal = _maxAdcVal;
    invert = _invert;
    deadZone = _deadZone;
    minOutput = -255;
    maxOutput = +255;
    pos = 0;
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
}

bool Lever::update()
{
    uint32_t currentTime = millis();

    // Check if the update interval has passed
    if (currentTime - lastUpdateTime > updateInterval)
    {
        // Update the last update time
        lastUpdateTime = currentTime;
        pos = readAndFilter();
        return true;
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

bool Lever::changed()
{
    if (pos != lastPos)
    {
        lastPos = pos; // Update lastPosition for the next check
        return true;
    }
    return false;
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

        // Use exponential function to smooth out the output
        calcRes = pow(calcRes, 3) / pow(maxOutput, 2);

        // Invert the value if needed
        if (invert)
            calcRes = -calcRes;
    }

    // Return result
    return calcRes;
}