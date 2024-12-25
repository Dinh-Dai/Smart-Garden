#include <Arduino.h>
#include <Ticker.h>
#include "secrets/wifi.h"
#include "wifi_connect.h"
#include "ca_cert.h"
#include "secrets/mqtt.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// WiFi credentials
const char *wifiSSID = WiFiSecrets::ssid;
const char *wifiPassword = WiFiSecrets::pass;

// MQTT broker settings
const char *mqttRelayTopic = "esp32/relay_manual";  // Topic for manual relay control
const char *mqttSoilTopic = "esp32/moisture";       // Topic for soil moisture data
const char *mqttLWTTopic = "esp32/lwt";
const char *mqttLWTMessage = "ESP32 unexpectedly disconnected.";
const int mqttKeepAlive = 15;
const int mqttSocketTimeout = 5;

// Pin definitions
const int pinSoilSensor = 34;
const int pinRelay1 = 26; // Relay controlled automatically by soil moisture
const int pinRelay2 = 27; // Relay controlled manually via MQTT

// Soil moisture configuration
const float soilThreshold = 80.0; // Soil moisture threshold (%)
const int soilMaxValue = 4095;    // Max ADC value for the soil sensor

// Ticker interval for publishing soil data
const int tickerInterval = 1000; // Interval in milliseconds

// MQTT client setup
WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);
Ticker soilDataTicker;

// Function prototypes
void onMQTTMessage(char *topic, byte *payload, unsigned int length);
void publishSoilData();
void reconnectMQTT();
void setupRelays();
void controlRelay1(float soilMoisture);

void setup()
{
    Serial.begin(115200);
    delay(100);

    // Pin setup
    pinMode(pinSoilSensor, INPUT);
    setupRelays();

    // Connect to WiFi
    setup_wifi(wifiSSID, wifiPassword);

    // Configure secure MQTT connection
    secureClient.setCACert(ca_cert);
    mqttClient.setServer(MQTT::broker, MQTT::port);
    mqttClient.setCallback(onMQTTMessage);

    // Start publishing soil data periodically
    soilDataTicker.attach_ms(tickerInterval, publishSoilData);
}

void loop()
{
    // Reconnect to MQTT if disconnected
    if (!mqttClient.connected())
    {
        reconnectMQTT();
    }
    mqttClient.loop();
}

// Function to initialize relays to default state
void setupRelays()
{
    pinMode(pinRelay1, OUTPUT);
    pinMode(pinRelay2, OUTPUT);
    digitalWrite(pinRelay1, HIGH); // Relay 1 (soil control) OFF by default
    digitalWrite(pinRelay2, HIGH); // Relay 2 (manual control) OFF by default
}

// Callback function for MQTT messages
void onMQTTMessage(char *topic, byte *payload, unsigned int length)
{
    Serial.printf("Message received on topic: %s\n", topic);

    String message = "";
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }
    Serial.printf("Payload: %s\n", message.c_str());

    // Handle manual relay control messages
    if (String(topic) == mqttRelayTopic)
    {
        if (message == "ON")
        {
            digitalWrite(pinRelay2, LOW); // Turn manual relay ON
            Serial.println("Manual Relay ON");
        }
        else if (message == "OFF")
        {
            digitalWrite(pinRelay2, HIGH); // Turn manual relay OFF
            Serial.println("Manual Relay OFF");
        }
    }
}

// Function to publish soil moisture data
void publishSoilData()
{
    int rawValue = analogRead(pinSoilSensor);
    float soilMoisture = (rawValue / float(soilMaxValue)) * 100;

    // Control relay 1 based on soil moisture
    controlRelay1(soilMoisture);

    // Publish soil moisture data to MQTT
    String moistureString = String(100 - soilMoisture, 2); // Reverse to get "dryness"
    mqttClient.publish(mqttSoilTopic, moistureString.c_str(), false);

    Serial.printf("Soil Moisture: %.2f%% (Published)\n", soilMoisture);
}

// Function to control relay 1 based on soil moisture
void controlRelay1(float soilMoisture)
{
    if (soilMoisture > soilThreshold)
    {
        digitalWrite(pinRelay1, LOW); // Turn relay 1 ON
        Serial.println("Relay 1 ON (Soil Moisture > Threshold)");
    }
    else
    {
        digitalWrite(pinRelay1, HIGH); // Turn relay 1 OFF
        Serial.println("Relay 1 OFF (Soil Moisture <= Threshold)");
    }
}

// Function to reconnect to MQTT
void reconnectMQTT()
{
    while (!mqttClient.connected())
    {
        Serial.println("Reconnecting to MQTT...");
        String clientID = "ESP32-" + String(WiFi.macAddress());
        if (mqttClient.connect(clientID.c_str(), MQTT::username, MQTT::password, mqttLWTTopic, 0, false, mqttLWTMessage))
        {
            Serial.printf("Connected to MQTT as %s\n", clientID.c_str());
            mqttClient.subscribe(mqttRelayTopic); // Subscribe to manual relay control topic
        }
        else
        {
            Serial.printf("Failed to connect, MQTT state: %d. Retrying in 1 second...\n", mqttClient.state());
            delay(1000);
        }
    }
}
