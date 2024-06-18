/**
 * @file wifi_ota_manager.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-18
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include "wifi_ota_manager.h"
#include <WiFi.h>
#include <ArduinoOTA.h>

#include "constants.h"

// Callback function to handle WiFi connection event
void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Connected to WiFi: " + WiFi.SSID());
}

// Callback function to handle IP address assignment event
void onWiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Got IP address: " + WiFi.localIP().toString());
}

// Callback function to handle WiFi disconnection event
void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Disconnected from WiFi: " + String(reinterpret_cast<char *>(info.wifi_sta_disconnected.ssid)));
}

// Setup Wi-Fi connection
void setupWiFi()
{
    // Register WiFi event handlers
    WiFi.onEvent(onWiFiConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(onWiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(onWiFiDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    // Bugfix for setting the hostname. More info: https://github.com/espressif/arduino-esp32/issues/3438
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.hostname(HOSTNAME);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

// Setup Arduino OTA (Over-The-Air) update
void setupOTA()
{
    // Arduino OTA initializing
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    // Callback functions for OTA events
    ArduinoOTA.onStart([]()
                       { Serial.println("OTA update started"); });
    // Blink with the built-in LED while updating
    ArduinoOTA.onProgress([](uint16_t progress, uint16_t total)
                          { digitalWrite(STATUS_LED, !digitalRead(STATUS_LED)); });
    // Turn off the built-in LED when update is finished
    ArduinoOTA.onEnd([]()
                     { digitalWrite(STATUS_LED, LOW);
                        Serial.println("OTA update finished"); });
    ArduinoOTA.begin();
}

void handleOTA()
{
    ArduinoOTA.handle();
}