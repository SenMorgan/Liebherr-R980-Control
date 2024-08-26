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
#define DISPLAY_TASK_FREQUENCY_HZ (50U)
#define DISPLAY_TASK_STACK_SIZE   (4 * 1024U)
#define DISPLAY_TASK_PRIORITY     (tskIDLE_PRIORITY + 1)
#define DISPLAY_TASK_CORE         1 // Core 0 is used by the WiFi

// Display dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

// Display addresses
#define LEFT_SCREEN_ADDRESS  0x3C
#define RIGHT_SCREEN_ADDRESS 0x3D

// Global variables
extern controller_data_struct dataToSend;
extern excavator_data_struct receivedData;
volatile bool displayEnabled = true;

// Semaphore to signal display disable
SemaphoreHandle_t displayDisableSemaphore;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 leftDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_SSD1306 rightDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

/**
 * @brief Disables the display.
 *
 * @param blocking If true, wait until the displays are disabled.
 */
void disableDisplay(bool blocking)
{
    displayEnabled = false;

    Serial.println("Disabling display...");

    if (blocking)
    {
        // Wait for the semaphore to be given by the display task
        if (xSemaphoreTake(displayDisableSemaphore, portMAX_DELAY) != pdTRUE)
        {
            Serial.println("Failed to disable display");
        }

        Serial.println("Display disabled");
    }
}

void setupDisplay(Adafruit_SSD1306 &display, uint8_t address)
{
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, address))
    {
        Serial.println(F("ERROR: SSD1306 allocation failed"));
        vTaskDelete(NULL);
    }
}

void printTitle(Adafruit_SSD1306 &display, const char *title, uint16_t batteryVoltage, uint16_t uptimeSec)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
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

/**
 * @brief Task for managing the displays.
 *
 * @param pvParameters A pointer to task parameters (not used in this function).
 */
void displayTask(void *pvParameters)
{
    const TickType_t xFrequency = pdMS_TO_TICKS(1000 / DISPLAY_TASK_FREQUENCY_HZ);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Wire.begin();

    Serial.println("displayTask started");

    setupDisplay(leftDisplay, LEFT_SCREEN_ADDRESS);
    setupDisplay(rightDisplay, RIGHT_SCREEN_ADDRESS);

    // Main task loop
    for (;;)
    {
        if (!displayEnabled)
        {
            leftDisplay.clearDisplay();
            leftDisplay.display();
            rightDisplay.clearDisplay();
            rightDisplay.display();

            // Give the semaphore to indicate that the display is disabled
            xSemaphoreGive(displayDisableSemaphore);

            // Wait for the display to be enabled
            while (!displayEnabled)
            {
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }

        printTitle(leftDisplay, "Controller", dataToSend.battery, millis() / 1000);
        printTitle(rightDisplay, "Excavator", receivedData.battery, receivedData.uptime);

        // Wait for the next cycle.
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
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
    displayDisableSemaphore = xSemaphoreCreateBinary();

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
