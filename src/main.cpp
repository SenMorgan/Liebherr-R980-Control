#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

#include "constants.h"
#include "data_structures.h"
#include "esp_now_manager.h"
#include "joysticks_manager.h"

// Create a variable to store the data that will be sent to the Excavator
controller_data_struct dataToSend;

void setup()
{
    // Setup pins
    setJoysticksPins();
    pinMode(LED_BUILTIN, OUTPUT);

    // Turn off the built-in LED
    digitalWrite(LED_BUILTIN, LOW);

    // Init Serial Monitor
    Serial.begin(115200);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    initEspNow();
}

void loop()
{
    readJoysticksPositions(dataToSend);
    sendDataToExcavator(dataToSend);

    // Wait some time before next iteration
    delay(1000);
}

