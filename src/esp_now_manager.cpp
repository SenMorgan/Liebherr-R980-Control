/**
 * @file esp_now_manager.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include "esp_now_manager.h"
#include <esp_now.h>
#include <WiFi.h>

#include "constants.h"
#include "data_structures.h"

// The MAC address of the Excavator got from platformio_override.ini
uint8_t excavatorMac[] = {EXCAVATOR_MAC};

// Create a variable to store the received data
excavator_data_struct receivedData;

// Create a structure to store the peer information
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void _onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    // Print error message if the data failed to send
    if (status != ESP_NOW_SEND_SUCCESS)
        Serial.printf("Data was not received by the Excavator\n");
}

// Callback when data is received
void _onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.printf("\nReceived from Excavator:\nUptime: %u\nBattery: %u\nCPU Temp: %.2f °C\n",
                  receivedData.uptime, receivedData.battery, receivedData.cpuTemp);
}

void initEspNow()
{
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Setup the peer
    memcpy(peerInfo.peer_addr, excavatorMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Add the peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }

    // Register for a callback function that will be called when data is sent
    esp_now_register_send_cb(_onDataSent);

    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(_onDataRecv);
}

void sendDataToExcavator(const controller_data_struct &data)
{
    // Send the data
    esp_err_t result = esp_now_send(excavatorMac, (uint8_t *)&data, sizeof(data));

    // Print error message if something went wrong
    if (result != ESP_OK)
        Serial.printf("Error sending data: %s\n", esp_err_to_name(result));
}