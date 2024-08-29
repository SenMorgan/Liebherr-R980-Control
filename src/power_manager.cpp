/**
 * @file power_manager.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-18
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include <data_structures.h>
#include "display.h"
#include "power_manager.h"

#include "constants.h"
#include "wifi_ota_manager.h"

// Formula to calculate the battery voltage (with a shorted diode)
#define CALCULATE_BATT_MV(mv) ((mv) * 6.92 + 337)

// LiPo battery voltage limits for this setup
#define MIN_BATT_MV 3200
#define MAX_BATT_MV 4200

// Global variables
extern controller_data_struct dataToSend;

uint32_t lastBatteryReadTime = 0;

/**
 * Function to put the system into deep sleep mode.
 * The board will wake up when the power button is pressed.
 */
void go_to_deep_sleep()
{
    // Configure the board to wake up when the power button is pressed
    Serial.println("Going to deep sleep mode...");
    esp_sleep_enable_ext0_wakeup(POWER_BUTTON, 0);

    // Enter deep sleep mode
    esp_deep_sleep_start();
}

void setupPowerManager(Button &powerBtn)
{
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    analogSetPinAttenuation(BATTERY_VOLTAGE_PIN, ADC_ATTENDB_MAX);
}

/**
 * Calculates the battery percentage using a sigmoidal function.
 *
 * @param voltage The input voltage value.
 * @return The sigmoidal value calculated based on the input voltage.
 *
 * @note Inspired by this repository: https://github.com/rlogiacco/BatterySense
 */
uint8_t calculateBatteryLevel(uint16_t voltage)
{
    if (voltage <= MIN_BATT_MV)
        return 0;
    if (voltage >= MAX_BATT_MV)
        return 100;

    uint8_t result = 105 - (105 / (1 + pow(1.724 * (voltage - MIN_BATT_MV) / (MAX_BATT_MV - MIN_BATT_MV), 5.5)));
    return result >= 100 ? 100 : result;
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
        Serial.println("Reading battery voltage...");

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

/**
 * @brief Checks the battery voltage and goes into deep sleep mode if it is too low.
 *
 * This function reads the battery voltage and checks if it is too low. If the battery voltage is below a certain threshold,
 * it displays a low power message on the displays, goes into deep sleep mode, and disables the display and status LED.
 */
void verifyBatteryLevel()
{
    // Read the battery voltage before going to sleep
    dataToSend.battery = readBatteryVoltage(false);
    if (dataToSend.battery < BATTERY_LOW_THRESHOLD)
    {
        Serial.println("Battery voltage is too low");
        // Show low power message on the displays
        setDisplayState(DISPLAY_LOW_POWER);
        // Go to deep sleep mode after delay
        delay(5000);
        disableDisplay();
        digitalWrite(STATUS_LED, LOW);
        go_to_deep_sleep();
    }
}
