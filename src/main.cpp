#include <array>
#include <WiFi.h>
#include <Wire.h>

#include "buttons_control.h"
#include "constants.h"
#include "data_structures.h"
#include "display.h"
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
    Lever(BOOM_LEVER, 10, 1010, true, 60),
    Lever(BUCKET_LEVER, 65, 1010, true, 70),
    Lever(STICK_LEVER, 10, 900, true),
    Lever(SWING_LEVER, 10, 930, false),
    Lever(LEFT_TRAVEL_LEVER, 10, 1010, true),
    Lever(RIGHT_TRAVEL_LEVER, 10, 1010, true)};

// Flags and variables
uint32_t lastSendDataTime = 0;
bool anyLeverMoved = false, leversCalibrated = false;

// Variable to track the last user activity time
volatile uint32_t lastUserActivityTime = millis();
volatile bool anyButtonPressed = false;

// Callback when data from Excavator received
void onDataFromExcavator(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.printf("\nReceived from Excavator:\nUptime: %u\nBattery: %u\n", receivedData.uptime, receivedData.battery);

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

// Callback function to handle power button press
void powerButtonCallback()
{
    // Update the last user activity time
    lastUserActivityTime = millis();

    if (powerBtn.action() == EB_CLICK)
    {
        Serial.println("Power button clicked - Turning off the board...");

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

/**
 * @brief Processes the button action and updates the button state.
 *
 * This function is called when a button is clicked. It updates the state of the button
 * and sets a flag indicating that a button has been pressed. It also updates the last user
 * activity time.
 *
 * @param buttonIndex The index of the button.
 * @param button The Button object representing the clicked button.
 */
void processButton(uint8_t buttonIndex, Button &button, const char *buttonName)
{
    if (button.action() == EB_CLICK)
    {
        dataToSend.buttonsStates[buttonIndex] = !dataToSend.buttonsStates[buttonIndex];
        Serial.printf("Button %s clicked\n", buttonName);
        anyButtonPressed = true;
        lastUserActivityTime = millis();
    }
}

/**
 * @brief Updates the positions of all levers and handles user activity.
 *
 * This function updates the positions of all levers and checks if any lever position has changed.
 * If any lever position has changed, it updates the last user activity time.
 * It also retrieves the positions of all levers if the board is powered, otherwise sets them to 0.
 */
void processLevers()
{
    if (leversCalibrated)
    {
        // Update all levers positions and recognize if any lever position has changed
        for (auto &lever : levers)
        {
            if (lever.update())
                anyLeverMoved = true;
        }

        // Update the last user activity time if any lever position has changed
        if (anyLeverMoved)
            lastUserActivityTime = millis();

        // Iterate over all levers and get their positions if the board is powered, otherwise set it to 0
        for (uint8_t i = 0; i < LEVERS_COUNT; i++)
            dataToSend.leverPositions[i] = levers[i].position();
    }
    else
    {
        // Calibrate all levers
        for (auto &lever : levers)
            lever.calibrate();

        // Set the flag indicating that the levers are calibrated
        leversCalibrated = true;
        Serial.println("Levers calibrated");
    }
}

/**
 * Checks if it is time to send data to the Excavator and sends the data if necessary.
 *
 * The function checks if any lever has moved or any button has been pressed. If so, it sends the data
 * to the Excavator every SEND_DATA_MIN_INTERVAL milliseconds. If no lever has moved or button has been
 * pressed, it sends the data every SEND_DATA_MAX_INTERVAL milliseconds.
 */
void checkAndSendData()
{
    bool timeToSendData = (anyLeverMoved || anyButtonPressed) && millis() - lastSendDataTime > SEND_DATA_MIN_INTERVAL;
    bool timeToPingExcavator = millis() - lastSendDataTime > SEND_DATA_MAX_INTERVAL;
    if (timeToSendData || timeToPingExcavator)
    {
        sendDataToExcavator(dataToSend);

        // Update the last send data time and reset the flags
        lastSendDataTime = millis();
        anyLeverMoved = false;
        anyButtonPressed = false;

        // Print all lever positions if any lever has moved
        Serial.printf("Boom: %3d | Bucket: %3d | Stick: %3d | Swing: %3d | "
                      "Track Left: %3d | Track Right: %3d | Lights: %d | Center Swing: %d | Battery: %3d\n",
                      dataToSend.leverPositions[0], dataToSend.leverPositions[1], dataToSend.leverPositions[2],
                      dataToSend.leverPositions[3], dataToSend.leverPositions[4], dataToSend.leverPositions[5],
                      dataToSend.buttonsStates[0], dataToSend.buttonsStates[1], dataToSend.battery);
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
    mainLightsBtn.attach([=]()
                         { processButton(0, mainLightsBtn, "Lights"); });
    centerSwingBtn.attach([=]()
                          { processButton(1, centerSwingBtn, "Center Swing"); });
    beaconLightModeBtn.attach([=]()
                              { processButton(2, beaconLightModeBtn, "Beacon Light Mode"); });

    // Turn ON the board and potentiometers power
    digitalWrite(BOARD_POWER, HIGH);

    // Setup callback for data received from Excavator
    setupDataRecvCallback(onDataFromExcavator);

    // Init displays
    displayTaskInit();

    // Init Wi-Fi and OTA
    setupWiFi();
    setupOTA();
    enableWiFi();

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
    processLevers();

    // Send data to the Excavator if necessary
    checkAndSendData();

    // Read battery voltage only after a period of inactivity not to disturb the user by disabling the Wi-Fi
    if (millis() - lastUserActivityTime > INACTIVITY_PERIOD_FOR_BATTERY_READ)
    {
        dataToSend.battery = readBatteryVoltage();
    }
}
