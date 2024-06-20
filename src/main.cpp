#include <WiFi.h>
#include <Wire.h>
#include <EncButton.h>

#include "constants.h"
#include "data_structures.h"
#include "esp_now_interface.h"
#include "joysticks_control.h"
#include "wifi_ota_manager.h"

// Create a variable to store the received data
excavator_data_struct receivedData;

// Create a variable to store the data that will be sent to the Excavator
controller_data_struct dataToSend;

Button powerBtn(POWER_BUTTON, INPUT_PULLUP);

volatile bool ledStatus = false;
volatile uint32_t lastDataReceivedTime = 0;

uint32_t lastJoystickReadTime = 0;

bool isBoardPowered = false;

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

void powerButtonCallback()
{
    switch (powerBtn.action())
    {
        case EB_CLICK:
            isBoardPowered = !isBoardPowered;
            digitalWrite(BOARD_POWER, isBoardPowered);
            Serial.println("Power button clicked");
            Serial.println(isBoardPowered ? "Board powered ON" : "Board powered OFF");
            break;
    }
}

void setup()
{
    // Setup pins
    setJoysticksPins();
    pinMode(STATUS_LED, OUTPUT);
    pinMode(BOARD_POWER, OUTPUT);

    // Turn on the built-in LED
    digitalWrite(STATUS_LED, HIGH);

    powerBtn.attach(powerButtonCallback);

    // Init Serial Monitor
    Serial.begin(115200);

    // Init Wi-Fi and OTA
    setupWiFi();
    setupOTA();

    // Init ESP-NOW
    initEspNow();

    // Register callback for data received from Excavator
    registerDataRecvCallback(onDataFromExcavator);

    // Finish initialization by logging message and turning off the built-in LED
    Serial.println(HOSTNAME + String(" initialized"));
    digitalWrite(STATUS_LED, LOW);
}

void loop()
{
    handleOTA();

    manageStatusLed();

    powerBtn.tick();

    // Read joysticks positions and send data to Excavator
    if (millis() - lastJoystickReadTime > JOYSTICK_READ_INTERVAL)
    {
        lastJoystickReadTime = millis();
        readJoysticksPositions(dataToSend);
        sendDataToExcavator(dataToSend);
    }
}
