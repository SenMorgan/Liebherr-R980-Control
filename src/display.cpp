/**
 * @file display.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-08-25
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <freertos/FreeRTOS.h>

#include <data_structures.h>
#include "display.h"

// Task parameters
#define DISPLAY_TASK_STACK_SIZE (4 * 1024U)
#define DISPLAY_TASK_PRIORITY   (tskIDLE_PRIORITY + 1)
#define DISPLAY_TASK_CORE       1 // Core 0 is used by the WiFi

// Display dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

// Display addresses
#define LEFT_SCREEN_ADDRESS  0x3C
#define RIGHT_SCREEN_ADDRESS 0x3D

// Global variables
extern controller_data_struct dataToSend;
extern excavator_data_struct receivedData;

DisplayState currentState = DISPLAY_DEFAULT;

// Semaphore to signal display disabled
SemaphoreHandle_t displayDisabledSemaphore;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 leftDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_SSD1306 rightDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

volatile uint16_t otaProgress;

/**
 * @brief Sets the display state.
 *
 * @param state The state to set the display to.
 */
void setDisplayState(DisplayState state)
{
    currentState = state;
    Serial.println("Display state changed to " + String(state));
}

/**
 * @brief Disables the display.
 *
 * @param blocking If true, wait until the displays are disabled.
 */
void disableDisplay(bool blocking)
{
    Serial.println("Disabling display...");
    currentState = DISPLAY_OFF;

    if (blocking)
    {
        // Wait for the semaphore to be given by the display task
        if (xSemaphoreTake(displayDisabledSemaphore, portMAX_DELAY) != pdTRUE)
        {
            Serial.println("ERROR: failed to disable display");
        }
        Serial.println("Display disabled");
    }
}

/**
 * @brief Power off the display (the lowest power consumption).
 *
 * @param display The display object to power off.
 */
void displayOff(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    display.display();
    display.ssd1306_command(SSD1306_DISPLAYOFF);
}

/**
 * @brief Initializes the display.
 *
 * This function sets up the display by initializing the Adafruit_SSD1306 object and configuring its properties.
 * It takes an Adafruit_SSD1306 object and the address of the display as parameters.
 *
 * @param display The Adafruit_SSD1306 object representing the display.
 * @param address The I2C address of the display.
 */
void setupDisplay(Adafruit_SSD1306 &display, uint8_t address)
{
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, address))
    {
        Serial.println(F("ERROR: SSD1306 allocation failed"));
        vTaskDelete(NULL);
    }
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
}

/**
 * @brief Sets the progress of the OTA (Over-The-Air) update.
 *
 * This function updates the value of the OTA progress variable that is used to display
 * the progress of the OTA update on the screen.
 *
 * @param percentage The progress percentage of the OTA update.
 */
void setOTAProgress(uint16_t percentage)
{
    otaProgress = percentage;
}

void printTitle(Adafruit_SSD1306 &display, const char *title, uint16_t batteryVoltage, uint16_t uptimeSec)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(title);

    // Calculate the battery level percentage (assuming 3.0V to 4.2V range)
    float batteryLevel = (batteryVoltage - 3000) / 12.0; // Convert mV to percentage

    // Draw the battery icon
    int batteryIconWidth = 16;
    int batteryIconHeight = 8;
    int batteryPoleWidth = 2;
    int batteryPoleHeight = 4;
    int batteryIconX = display.width() - batteryIconWidth - batteryPoleWidth;
    int batteryIconY = 0;

    // Draw the battery outline
    display.drawRect(batteryIconX, batteryIconY, batteryIconWidth, batteryIconHeight, SSD1306_WHITE);

    // Draw the battery pole
    display.fillRect(batteryIconX + batteryIconWidth, batteryIconHeight / 2 - batteryPoleHeight / 2,
                     batteryPoleWidth, batteryPoleHeight, SSD1306_WHITE);

    // Draw the battery level
    int batteryLevelWidth = (batteryIconWidth - 2) * (batteryLevel / 100.0);
    display.fillRect(batteryIconX + 1, batteryIconY + 1, batteryLevelWidth, batteryIconHeight - 2, SSD1306_WHITE);

    // Draw the battery voltage
    int batteryVoltageWidth = 30;
    display.setCursor(batteryIconX - batteryVoltageWidth - 5, batteryIconY);
    display.setTextSize(1);
    display.print(batteryVoltage / 1000.0, 2); // Print voltage in Volts
    display.print("V");

    // Draw the uptime on the next line
    display.setCursor(0, 8);
    display.print("Uptime: ");
    display.print(uptimeSec);

    display.display();
}

// ############################## Screens ##############################
void displayDefault()
{
    printTitle(leftDisplay, "CONTROLLER", dataToSend.battery, millis() / 1000);
    printTitle(rightDisplay, "EXCAVATOR", receivedData.battery, receivedData.uptime);
}

void displayLowPower()
{
    static bool blinkState = true;

    // Blink the low power message
    leftDisplay.clearDisplay();
    leftDisplay.setTextSize(2);
    leftDisplay.setCursor(0, 0);
    leftDisplay.print("BAT: ");
    leftDisplay.print(dataToSend.battery / 1000.0, 2); // Print voltage in Volts
    leftDisplay.print("V");
    if (blinkState)
    {
        leftDisplay.setCursor(38, 20);
        leftDisplay.setTextSize(3);
        leftDisplay.print("LOW");
    }

    rightDisplay.clearDisplay();
    rightDisplay.setTextSize(2);
    rightDisplay.setCursor(0, 0);
    rightDisplay.print("BAT: ");
    rightDisplay.print(dataToSend.battery / 1000.0, 2); // Print voltage in Volts
    rightDisplay.print("V");
    if (blinkState)
    {
        rightDisplay.setCursor(20, 20);
        rightDisplay.setTextSize(3);
        rightDisplay.print("POWER");
    }

    leftDisplay.display();
    rightDisplay.display();

    // Toggle the blink state
    blinkState = !blinkState;
}

void displayOtaUpdate()
{
    // Clear the displays
    leftDisplay.clearDisplay();
    rightDisplay.clearDisplay();

    // Print OTA update message
    leftDisplay.setTextSize(2);
    leftDisplay.setCursor(5, 0);
    leftDisplay.print("OTA UPDATE");

    rightDisplay.setTextSize(2);
    rightDisplay.setCursor(5, 0);
    rightDisplay.print("IN PROCESS");

    // Draw progress bar
    uint16_t barWidth = 100;
    uint16_t barHeight = 10;
    uint16_t barX = (leftDisplay.width() - barWidth) / 2;
    uint16_t barY = (leftDisplay.height() - barHeight) / 2;

    // Calculate the fill width based on the progress percentage
    uint16_t barFillWidth = (barWidth - 2) * ((float)otaProgress / 100);

    leftDisplay.drawRect(barX, barY, barWidth, barHeight, SSD1306_WHITE);
    leftDisplay.fillRect(barX + 1, barY + 1, barFillWidth, barHeight - 2, SSD1306_WHITE);

    rightDisplay.drawRect(barX, barY, barWidth, barHeight, SSD1306_WHITE);
    rightDisplay.fillRect(barX + 1, barY + 1, barFillWidth, barHeight - 2, SSD1306_WHITE);

    // Print progress percentage
    uint16_t posY = barY + barHeight + 5;
    String progressText = String(otaProgress) + "%";
    int16_t x1, y1;
    uint16_t textWidth, textHeight;

    // Calculate the width of the progress text
    leftDisplay.getTextBounds(progressText, 0, 0, &x1, &y1, &textWidth, &textHeight);
    leftDisplay.setCursor((leftDisplay.width() - textWidth) / 2, posY);
    leftDisplay.print(progressText);

    rightDisplay.getTextBounds(progressText, 0, 0, &x1, &y1, &textWidth, &textHeight);
    rightDisplay.setCursor((rightDisplay.width() - textWidth) / 2, posY);
    rightDisplay.print(progressText);

    // Update the displays
    leftDisplay.display();
    rightDisplay.display();
}

/**
 * @brief Task for managing the displays.
 *
 * @param pvParameters A pointer to task parameters (not used in this function).
 */
void displayTask(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    bool displayEnabled = true;

    // Initialize the I2C bus
    Wire.begin();

    // Setup the displays
    setupDisplay(leftDisplay, LEFT_SCREEN_ADDRESS);
    setupDisplay(rightDisplay, RIGHT_SCREEN_ADDRESS);

    Serial.println("displayTask started");

    // Main task loop
    for (;;)
    {
        // Update displays based on the current state
        switch (currentState)
        {
            case DISPLAY_OFF:
                if (displayEnabled)
                {
                    // Power off the displays
                    displayOff(leftDisplay);
                    displayOff(rightDisplay);
                    // Give the semaphore to indicate that the display is disabled
                    xSemaphoreGive(displayDisabledSemaphore);
                    displayEnabled = false;
                }
                // Idle task
                xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
                break;
            case DISPLAY_DEFAULT:
                displayDefault();
                // Delay to control the refresh rate
                xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
                break;
            case DISPLAY_LOW_POWER:
                displayLowPower();
                // Delay to control the blink rate
                xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
                break;
            case DISPLAY_OTA_UPDATE:
                displayOtaUpdate();
                // Delay to control the blink rate
                xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
                break;
        }
    }
}

/**
 * @brief Initializes the display task.
 *
 * This function creates a new task called "displayTask" and assigns it to a specific core.
 *
 * @note This function should be called once during the setup phase of the program.
 */
void displayTaskInit(void)
{
    // Create the semaphore
    displayDisabledSemaphore = xSemaphoreCreateBinary();

    if (pdPASS != xTaskCreatePinnedToCore(displayTask,
                                          "displayTask",
                                          DISPLAY_TASK_STACK_SIZE,
                                          NULL,
                                          DISPLAY_TASK_PRIORITY,
                                          NULL,
                                          DISPLAY_TASK_CORE))
    {
        Serial.println("Failed to create displayTask");
    }
}
