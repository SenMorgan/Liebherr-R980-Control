#include <array>
#include <WiFi.h>
#include <Wire.h>

#include "buttons_control.h"
#include "constants.h"
#include "data_structures.h"
#include "esp_now_interface.h"
#include "leds.h"
#include "lever_control.h"
#include "power_manager.h"
#include "wifi_ota_manager.h"

// Structure to store the data received from the Excavator
excavator_data_struct receivedData;

// Structure to store the data to be sent to the Excavator
controller_data_struct dataToSend;

// Levers
std::array<Lever, LEVERS_COUNT> levers = {
    Lever(BOOM_LEVER, 10, 1010, true),
    Lever(BUCKET_LEVER, 65, 1010, true),
    Lever(STICK_LEVER, 10, 900, true),
    Lever(SWING_LEVER, 10, 930, false),
    Lever(LEFT_TRAVEL_LEVER, 10, 1010, true),
    Lever(RIGHT_TRAVEL_LEVER, 10, 1010, true)};

// Flags and variables
uint32_t lastSendDataTime = 0;
bool isBoardPowered = true;

// Variable to track the last user activity time
volatile uint32_t lastUserActivityTime = millis();

// Callback when data from Excavator received
void onDataFromExcavator(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    // Serial.printf("\nReceived from Excavator:\nUptime: %u\nBattery: %u\nCPU Temp: %.2f °C\n",
    //               receivedData.uptime, receivedData.battery, (float)receivedData.cpuTemp / 100.0);

    // Blink the LED to indicate data received
    blinkWithLed(LED_BUTTON_B);
}

void zeroLeversPositions()
{
    // Set all levers positions to 0
    for (uint8_t i = 0; i < LEVERS_COUNT; i++)
        dataToSend.leverPositions[i] = 0;

    // Send the data to the Excavator
    sendDataToExcavator(dataToSend);

    // Delay to allow the ESP-NOW to send the data
    delay(100);
}

void manageLevers()
{
    bool anyLeverChanged = false;

    // Update all levers positions and recognize if any lever position has changed
    for (auto &lever : levers)
    {
        if (lever.update())
            anyLeverChanged = true;
    }

    // Update the last user activity time if any lever position has changed
    if (anyLeverChanged)
        lastUserActivityTime = millis();

    // If any lever position has changed and the last data send time is greater than the minimum interval
    // or the maximum interval has passed since the last data send, send the data to the Excavator.
    if ((anyLeverChanged && millis() - lastSendDataTime > SEND_DATA_MIN_INTERVAL) ||
        millis() - lastSendDataTime > SEND_DATA_MAX_INTERVAL)
    {
        lastSendDataTime = millis();

        // Iterate over all levers and get their positions if the board is powered, otherwise set it to 0
        for (uint8_t i = 0; i < LEVERS_COUNT; i++)
            dataToSend.leverPositions[i] = isBoardPowered ? levers[i].position() : 0;

        sendDataToExcavator(dataToSend);
    }
}

// Callback function to handle power button press
void powerButtonCallback()
{
    // Update the last user activity time
    lastUserActivityTime = millis();

    if (powerBtn.action() == EB_CLICK)
    {
        Serial.println("Power button clicked");
        isBoardPowered = !isBoardPowered;
        if (isBoardPowered)
        {
            Serial.println("Board powered ON");

            // Turn ON the board and potentiometers power
            digitalWrite(BOARD_POWER, HIGH);

            // Some delay to stabilize the power
            delay(100);

            // Calibrate all levers after power ON
            for (auto &lever : levers)
                lever.calibrate();

            // Enable Wi-Fi and power ON the board
            enableWiFi();
        }
        else
        {
            Serial.println("Board powered OFF");

            // Zero all levers positions
            zeroLeversPositions();

            // Disable Wi-Fi
            disableWiFi();

            // Turn OFF the board and LEDs power
            digitalWrite(BOARD_POWER, LOW);
            digitalWrite(LED_BUTTON_A, LOW);
            digitalWrite(LED_BUTTON_B, LOW);
            digitalWrite(LED_BUTTON_C, LOW);

            // Go to deep sleep mode
            go_to_deep_sleep();
        }
    }
}

void setup()
{
    // Setup pins
    pinMode(STATUS_LED, OUTPUT);
    pinMode(BOARD_POWER, OUTPUT);
    pinMode(LED_BUTTON_A, OUTPUT);
    pinMode(LED_BUTTON_B, OUTPUT);
    pinMode(LED_BUTTON_C, OUTPUT);

    // Turn on the built-in LED to indicate initialization
    digitalWrite(STATUS_LED, HIGH);

    // Init Serial Monitor
    Serial.begin(115200);

    // Setup power manager and read battery voltage during startup
    setupPowerManager(powerBtn);
    dataToSend.battery = readBatteryVoltage(false);

    // Init buttons
    initButtons();
    powerBtn.attach(powerButtonCallback);

    // Turn ON the board and potentiometers power
    digitalWrite(BOARD_POWER, HIGH);

    // Setup callback for data received from Excavator
    setupDataRecvCallback(onDataFromExcavator);

    // Init Wi-Fi and OTA
    setupWiFi();
    setupOTA();
    enableWiFi();

    // Calibrate all levers
    for (auto &lever : levers)
        lever.calibrate();

    // Finish initialization by logging message and turning off the built-in LED
    Serial.printf("\n%s [%s] initialized\n", HOSTNAME, WiFi.macAddress().c_str());
    digitalWrite(STATUS_LED, LOW);
}

void loop()
{
    handleOTA();

    // Handle buttons
    tickButtons();

    // Get the lever positions and send the data to the Excavator
    manageLevers();

    // Read battery voltage only after a period of inactivity not to disturb the user by disabling the Wi-Fi
    if (millis() - lastUserActivityTime > INACTIVITY_PERIOD_FOR_BATTERY_READ)
    {
        dataToSend.battery = readBatteryVoltage(isBoardPowered);
    }
}
