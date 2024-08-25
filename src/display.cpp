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
#include "display.h"

// Task parameters
#define DISPLAY_TASK_FREQUENCY_HZ (50U)
#define DISPLAY_TASK_STACK_SIZE   (4 * 1024U)
#define DISPLAY_TASK_PRIORITY     (tskIDLE_PRIORITY + 1)
#define DISPLAY_TASK_CORE         1 // Core 0 is used by the WiFi

#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define LEFT_SCREEN_ADDRESS  0x3C
#define RIGHT_SCREEN_ADDRESS 0x3D

Adafruit_SSD1306 leftDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_SSD1306 rightDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void setupDisplay(Adafruit_SSD1306 &display, uint8_t address)
{
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, address))
    {
        Serial.println(F("ERROR: SSD1306 allocation failed"));
        vTaskDelete(NULL);
    }
}

void printTitle(Adafruit_SSD1306 &display, const char *title)
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(title);
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

    printTitle(leftDisplay, "Controller");
    printTitle(rightDisplay, "Excavator");

    // Main task loop
    for (;;)
    {
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
