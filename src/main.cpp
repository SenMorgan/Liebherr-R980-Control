#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

#include "constants.h"
#include "data_structures.h"
#include "esp_now_manager.h"
#include "joysticks_manager.h"

// Create a variable to store the received data
excavator_data_struct receivedData;

// Create a variable to store the data that will be sent to the Excavator
controller_data_struct dataToSend;

volatile bool ledStatus = false;
volatile uint32_t lastDataReceivedTime = 0;

uint32_t lastJoystickReadTime = 0;

// Callback when data from Excavator received
void onDataFromExcavator(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.printf("\nReceived from Excavator:\nUptime: %u\nBattery: %u\nCPU Temp: %.2f °C\n",
                  receivedData.uptime, receivedData.battery, receivedData.cpuTemp);

    // Turn on the built-in LED, set flag and save the last time data was received
    digitalWrite(STATUS_LED, HIGH);
    ledStatus = true;
    lastDataReceivedTime = millis();
}

void manageStatusLed()
{
    // Turn off the built-in LED after some time if was turned on
    if (ledStatus && millis() - lastDataReceivedTime > STATUS_LED_BLINK_PERIOD)
    {
        digitalWrite(STATUS_LED, LOW);
        ledStatus = false;
    }
}

void setup()
{
    // Setup pins
    setJoysticksPins();
    pinMode(STATUS_LED, OUTPUT);

    // Turn off the built-in LED
    digitalWrite(STATUS_LED, LOW);

    // Init Serial Monitor
    Serial.begin(115200);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    initEspNow();

    // Register for a callback function that will be called when data is received
    registerDataRecvCallback(onDataFromExcavator);
}

void loop()
{
    manageStatusLed();

    // Read joysticks positions and send data to Excavator
    if (millis() - lastJoystickReadTime > JOYSTICK_READ_PERIOD)
    {
        lastJoystickReadTime = millis();
        readJoysticksPositions(dataToSend);
        sendDataToExcavator(dataToSend);
    }
}
