/**
 * @file esp_now_interface.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include "esp_now_interface.h"
#include <esp_now.h>
#include <WiFi.h>

#include "constants.h"
#include "data_structures.h"
#include "leds.h"

// The MAC address of the Excavator got from platformio_override.ini
uint8_t excavatorMac[] = {EXCAVATOR_MAC};

// Create a structure to store the peer information
esp_now_peer_info_t peerInfo;

// Variable to store a callback when data were received
esp_now_recv_cb_t onDataReceivedCallback = NULL;

// Callback when data is sent
void _onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    // Indicate that the data was sent even if it possibly failed
    blinkWithLed(LED_BUTTON_A);

    // Print error message if the data failed to send
    if (status != ESP_NOW_SEND_SUCCESS)
        Serial.printf("Data was not received by the Excavator\n");
}

void setupDataRecvCallback(esp_now_recv_cb_t callback)
{
    onDataReceivedCallback = callback;
}

void setupPeer()
{
    // Check if the peer has already been set
    static boolean peerSet = false;

    if (peerSet)
    {
        // Peer has already been set - return
        return;
    }

    // Setup the peer
    memcpy(peerInfo.peer_addr, excavatorMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    peerSet = true;
}

void initEspNow()
{
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Set up the peer if it hasn't been set up yet
    setupPeer();

    // Add the peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }

    // Register for a callback function that will be called when data is sent
    esp_now_register_send_cb(_onDataSent);

    esp_now_register_recv_cb(onDataReceivedCallback);
}

void sendDataToExcavator(const controller_data_struct &data)
{
#define NO_MEM_RETRY_INTERVAL 1000
    static unsigned long lastSendTime = 0;
    static bool awaitingRetry = false;
    static bool wasMemErrorLastTime = false;

    // Continue only if WiFi interface is in valid mode
    if (WiFi.getMode() != WIFI_AP_STA && WiFi.getMode() != WIFI_AP)
        return;

    // Check if we are waiting to retry and if the wait time has elapsed
    if (awaitingRetry && millis() - lastSendTime < NO_MEM_RETRY_INTERVAL)
        return; // Exit the function early if we are still waiting to retry

    // Attempt to send the data
    esp_err_t result = esp_now_send(excavatorMac, (uint8_t *)&data, sizeof(data));

    // Handle the result of the send attempt
    if (result == ESP_ERR_ESPNOW_NO_MEM)
    {
        if (wasMemErrorLastTime)
        {
            // Log the error if this is the second consecutive ESP_ERR_ESPNOW_NO_MEM
            Serial.printf("Error sending data after retry: %s\n", esp_err_to_name(result));
        }
        // Set up for a retry
        lastSendTime = millis();
        awaitingRetry = true;
        wasMemErrorLastTime = true;
    }
    else
    {
        // If the send was successful or failed for a reason other than ESP_ERR_ESPNOW_NO_MEM, reset retry state
        awaitingRetry = false;
        wasMemErrorLastTime = false;
        if (result != ESP_OK)
        {
            // Log the error if it's not ESP_OK and not ESP_ERR_ESPNOW_NO_MEM
            Serial.printf("Error sending data: %s\n", esp_err_to_name(result));
        }
    }
}