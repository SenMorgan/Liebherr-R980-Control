#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

// Excavator MAC Address got from platformio_override.ini
uint8_t excavatorMac[] = {EXCAVATOR_MAC};

// Define the structure type that will be sent over ESP-NOW from the Controller to the Excavator
typedef struct to_excavator_struct
{
    int var;
} to_excavator_struct;

// Define the structure type that will be sent over ESP-NOW from the Excavator to the Controller
typedef struct from_excavator_struct
{
    int var;
} from_excavator_struct;

// Create variables of the structures
to_excavator_struct outgoingReadings;
from_excavator_struct incomingReadings;

// Create a structure to store the peer information
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Int received: ");
    Serial.println(incomingReadings.var);
    Serial.println();
}

void setup()
{
    // Init Serial Monitor
    Serial.begin(115200);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for Send CB to get the status of Transmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, excavatorMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }

    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

    outgoingReadings.var = 0;
}

void loop()
{
    // Send data
    esp_err_t result = esp_now_send(excavatorMac, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));
    if (result == ESP_OK)
    {
        Serial.println("Sent with success");
    }
    else
    {
        Serial.println("Error sending the data");
    }

    outgoingReadings.var++;

    delay(1000);
}
