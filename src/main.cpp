#include <array>
#include <WiFi.h>
#include <Wire.h>

#include "buttons_control.h"
#include "constants.h"
#include "data_structures.h"
#include "esp_now_interface.h"
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
volatile bool ledStatus = false;
volatile uint32_t lastDataReceivedTime = 0;

uint32_t lastSendDataTime = 0;

bool isBoardPowered = false;

// Variable to track the last user activity time
volatile uint32_t lastUserActivityTime = millis();

// Callback when data from Excavator received
void onDataFromExcavator(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    // Serial.printf("\nReceived from Excavator:\nUptime: %u\nBattery: %u\nCPU Temp: %.2f °C\n",
    //               receivedData.uptime, receivedData.battery, (float)receivedData.cpuTemp / 100.0);

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

void powerOnBoard()
{
    // Turn ON the board and potentiometers power
    digitalWrite(BOARD_POWER, HIGH);
    isBoardPowered = true;
    Serial.println("Board powered ON");

    // Some delay to stabilize the power
    delay(100);

    // Calibrate all levers after power ON
    for (auto &lever : levers)
    {
        lever.calibrate();
    }
}

void powerOffBoard()
{
    // Turn OFF the board and potentiometers power
    digitalWrite(BOARD_POWER, LOW);
    isBoardPowered = false;
    Serial.println("Board powered OFF");

    // Turn off all LEDs
    digitalWrite(LED_BUTTON_A, LOW);
    digitalWrite(LED_BUTTON_B, LOW);
    digitalWrite(LED_BUTTON_C, LOW);
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
        isBoardPowered ? powerOnBoard() : powerOffBoard();
    }
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

void ledsAnimation()
{
#define BLINK_INTERVAL 250

    static uint32_t lastBlinkTime = 0;
    static uint8_t currentLed = LED_BUTTON_C; // Start with LED A

    if (isBoardPowered && millis() - lastBlinkTime >= BLINK_INTERVAL)
    {
        lastBlinkTime = millis(); // Update the last blink time

        // Turn off all LEDs
        digitalWrite(LED_BUTTON_A, LOW);
        digitalWrite(LED_BUTTON_B, LOW);
        digitalWrite(LED_BUTTON_C, LOW);

        // Turn on the next LED
        digitalWrite(currentLed, HIGH);

        // Move to the next LED in sequence (from C to A)
        switch (currentLed)
        {
            case LED_BUTTON_A:
                currentLed = LED_BUTTON_C;
                break;
            case LED_BUTTON_B:
                currentLed = LED_BUTTON_A;
                break;
            case LED_BUTTON_C:
                currentLed = LED_BUTTON_B;
                break;
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

    // Setup callback for data received from Excavator
    setupDataRecvCallback(onDataFromExcavator);

    // Init Wi-Fi and OTA
    setupWiFi();
    setupOTA();

    // Power ON the board
    powerOnBoard();

    // Finish initialization by logging message and turning off the built-in LED
    Serial.println(HOSTNAME + String(" initialized"));
    digitalWrite(STATUS_LED, LOW);
}

void loop()
{
    handleOTA();

    manageStatusLed();

    // Handle buttons
    tickButtons();

    ledsAnimation();

    manageLevers();

    // Read battery voltage only after a period of inactivity not to disturb the user by disabling the Wi-Fi
    if (millis() - lastUserActivityTime > INACTIVITY_PERIOD_FOR_BATTERY_READ)
    {
        dataToSend.battery = readBatteryVoltage();
    }
}
