/**
 * @file power_manager.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-18
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include "power_manager.h"

#include "constants.h"
#include "wifi_ota_manager.h"

// Formula to calculate the battery voltage (with a shorted diode)
#define CALCULATE_BATT_MV(mv) ((mv) * 7.35 + 410)

uint32_t lastBatteryReadTime = 0;

void setupPowerManager(Button &powerBtn)
{
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    analogSetPinAttenuation(BATTERY_VOLTAGE_PIN, ADC_11db);
}

uint16_t getAveragedBattVoltage()
{
// Define the number of readings
#define NUM_READINGS 5

    uint32_t total = 0;

    // Take multiple readings
    for (int i = 0; i < NUM_READINGS; ++i)
    {
        total += analogRead(BATTERY_VOLTAGE_PIN);
        delay(10); // Short delay between readings
    }

    // Calculate the average reading
    uint16_t avgReadMv = total / NUM_READINGS;

    // Calculate the battery voltage using formula
    uint16_t battMv = CALCULATE_BATT_MV(avgReadMv);

    return battMv;
}

uint16_t readBatteryVoltage(bool reEnableWiFi)
{
    static uint16_t battMv = 0;

    // Read the battery voltage every BATTERY_READ_INTERVAL milliseconds or if it's the first run
    if (millis() - lastBatteryReadTime >= BATTERY_READ_INTERVAL || lastBatteryReadTime == 0)
    {
        lastBatteryReadTime = millis();

        // If reenableWiFi is true, disable the WiFi before reading the battery voltage
        if (reEnableWiFi)
        {
            disableWiFi();
        }
        // If the WiFi is enabled and reenableWiFi is false, print an error message and return
        else if (isWiFiEnabled())
        {
            Serial.println("WiFi must be disabled to read the battery voltage!");
            return 0;
        }

        // Reset the value so we can detect if there was an error reading the battery voltage
        battMv = 0;

        // Read the battery voltage
        battMv = getAveragedBattVoltage();
        Serial.printf("Battery Voltage: %u mV\n", battMv);

        // Re-enable the WiFi if needed
        if (reEnableWiFi)
            enableWiFi();
    }

    return battMv;
}