#include <Arduino.h>
#include <Ticker.h>
#include "secrets/wifi.h"
#include "wifi_connect.h"
#include "ca_cert.h"
#include "secrets/mqtt.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

// WiFi credentials
const char *wifiSSID = WiFiSecrets::ssid;
const char *wifiPassword = WiFiSecrets::pass;

// MQTT broker settings
const char *mqttSoilTopic = "esp32/moisture";           // Topic for soil moisture data
const char *mqttTemperatureTopic = "esp32/temperature"; // Topic for temperature data
const char *mqttHumidityTopic = "esp32/humidity";       // Topic for humidity data
const char *mqttLightTopic = "esp32/intensity";         // Topic for light intensity data
const char *mqttLWTTopic = "esp32/lwt";
const char *mqttLWTMessage = "ESP32 unexpectedly disconnected.";
const int mqttKeepAlive = 15;
const int mqttSocketTimeout = 5;

// Updated MQTT topics for new devices
const char *pumpModeTopic = "esp32/pump_mode";
const char *pumpControlTopic = "esp32/pump_control";
const char *pumpStatusTopic = "esp32/pump_status";
const char *pumpTimeTopic = "esp32/pump_time";
const char *ledModeTopic = "esp32/led_mode";
const char *ledControlTopic = "esp32/led_control";
const char *ledStatusTopic = "esp32/led_status";
const char *ledTimeTopic = "esp32/led_time";
const char *fanModeTopic = "esp32/fan_mode";
const char *fanControlTopic = "esp32/fan_control";
const char *fanStatusTopic = "esp32/fan_status";
const char *fanTimeTopic = "esp32/fan_time";

// Topics for setting thresholds
const char *setTemperatureTopic = "esp32/set_temperature";
const char *setMoistureTopic = "esp32/set_moisture";
const char *setHumidityTopic = "esp32/set_humidity";
const char *setIntensityTopic = "esp32/set_intensity";

// Pin definitions
const int pinPump = 26;          // Pin for pump relay
const int pinLED = 27;           // Pin for LED relay
const int pinFan = 5;            // Pin for fan relay
const int pinSoilSensor = 34;    // Pin for soil moisture sensor
const int pinLightSensor = 32;   // Pin for light sensor

// Default thresholds
float temperatureThreshold = 30.0;  // Temperature threshold (°C)
float soilThreshold = 40.0;         // Soil moisture threshold (%)
float humidityThreshold = 80.0;     // Humidity threshold (%)
int lightThreshold = 300;           // Light intensity threshold

// DHT sensor configuration
#define DHTPIN 4                   // Pin for DHT11 sensor
#define DHTTYPE DHT11              // Type of DHT sensor (DHT11)
DHT dht(DHTPIN, DHTTYPE);          // Initialize DHT sensor

// Ticker interval for publishing sensor data
const int tickerInterval = 1000; 

// MQTT client setup
WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);
Ticker sensorDataTicker;

// Function prototypes
void onMQTTMessage(char *topic, byte *payload, unsigned int length);
void publishSensorData();
void reconnectMQTT();
void setupDevices();
void controlPump(float soilMoisture);
void controlLED(int lightLevel);
void controlFan(float temperature, float humidity);
void publishDeviceStatus(const char *topic, bool status);
void publishDeviceActiveTime(const char *topic, unsigned long activeTime);

// Global variables to store the current mode
bool isPumpAutoMode = false;
bool isLEDAutoMode = false;
bool isFanAutoMode = false;

// Global variables to store the start time and total active time for devices
unsigned long pumpStartTime = 0;
unsigned long ledStartTime = 0;
unsigned long fanStartTime = 0;
unsigned long pumpActiveTime = 0;
unsigned long ledActiveTime = 0;
unsigned long fanActiveTime = 0;

void setup(){
    Serial.begin(115200);
    delay(100);
    setupDevices();                         // Pin setup
    setup_wifi(wifiSSID, wifiPassword);     // Connect to WiFi
    // Configure secure MQTT connection
    secureClient.setCACert(ca_cert);
    mqttClient.setServer(MQTT::broker, MQTT::port);
    mqttClient.setCallback(onMQTTMessage);
    dht.begin();                            // Start DHT sensor
    // Start publishing sensor data periodically
    sensorDataTicker.attach_ms(tickerInterval, publishSensorData);
}

void loop(){
    // Reconnect to MQTT if disconnected
    if (!mqttClient.connected()){
        reconnectMQTT();
    }
    mqttClient.loop();
}

// Function to initialize devices to default state
void setupDevices(){
    pinMode(pinPump, OUTPUT);
    pinMode(pinLED, OUTPUT);
    pinMode(pinFan, OUTPUT);
    pinMode(pinSoilSensor, INPUT);
    pinMode(pinLightSensor, INPUT);
    digitalWrite(pinPump, HIGH); 
    digitalWrite(pinLED, HIGH); 
    digitalWrite(pinFan, HIGH); 
}

// Callback function for MQTT messages
void onMQTTMessage(char *topic, byte *payload, unsigned int length){
    String message = "";
    for (unsigned int i = 0; i < length; i++){
        message += (char)payload[i];
    }
    // Handle pump mode control messages
    if (String(topic) == pumpModeTopic){
        if (message == "AUTO"){
            isPumpAutoMode = true;
        }
        else if (message == "MANUAL"){
            isPumpAutoMode = false;
        }
    }
    // Handle pump control messages
    else if (String(topic) == pumpControlTopic){
        if (!isPumpAutoMode){                 // Only allow manual control if not in AUTO mode
            if (message == "ON"){
                digitalWrite(pinPump, LOW);   // Turn pump ON
                pumpStartTime = millis();     // Record start time
                publishDeviceStatus(pumpStatusTopic, true);
            }
            else if (message == "OFF"){
                digitalWrite(pinPump, HIGH);   // Turn pump OFF
                pumpActiveTime = (millis() - pumpStartTime) / 1000;   // Calculate active time in seconds
                publishDeviceStatus(pumpStatusTopic, false);
                publishDeviceActiveTime(pumpTimeTopic, pumpActiveTime); // Publish active time
            }
        }
    }
    // Handle LED mode control messages
    else if (String(topic) == ledModeTopic){
        if (message == "AUTO"){
            isLEDAutoMode = true;
        }
        else if (message == "MANUAL"){
            isLEDAutoMode = false;
        }
    }
    // Handle LED control messages
    else if (String(topic) == ledControlTopic){
        if (!isLEDAutoMode){          // Only allow manual control if not in AUTO mode
            if (message == "ON"){
                digitalWrite(pinLED, LOW);      // Turn LED ON
                ledStartTime = millis();        // Record start time
                publishDeviceStatus(ledStatusTopic, true);
            }
            else if (message == "OFF"){
                digitalWrite(pinLED, HIGH);       // Turn LED OFF
                ledActiveTime = (millis() - ledStartTime) / 1000; // Calculate active time in seconds
                publishDeviceStatus(ledStatusTopic, false);
                publishDeviceActiveTime(ledTimeTopic, ledActiveTime); // Publish active time
            }
        }
    }
    // Handle fan mode control messages
    else if (String(topic) == fanModeTopic){
        if (message == "AUTO"){
            isFanAutoMode = true;
            Serial.println("Fan mode set to AUTO");
        }
        else if (message == "MANUAL"){
            isFanAutoMode = false;
            Serial.println("Fan mode set to MANUAL");
        }
    }
    // Handle fan control messages
    else if (String(topic) == fanControlTopic){
        if (!isFanAutoMode){ // Only allow manual control if not in AUTO mode
            if (message == "ON"){
                digitalWrite(pinFan, LOW);        // Turn fan ON
                fanStartTime = millis();          // Record start time
                publishDeviceStatus(fanStatusTopic, true);
            }
            else if (message == "OFF"){
                digitalWrite(pinFan, HIGH);        // Turn fan OFF
                fanActiveTime = (millis() - fanStartTime) / 1000; // Calculate active time in seconds
                publishDeviceStatus(fanStatusTopic, false);
                publishDeviceActiveTime(fanTimeTopic, fanActiveTime); // Publish active time
            }
        }
    }
    // Handle setting temperature threshold
    else if (String(topic) == setTemperatureTopic){
        temperatureThreshold = message.toFloat();
    }
    // Handle setting moisture threshold
    else if (String(topic) == setMoistureTopic){
        soilThreshold = message.toFloat();
    }
    // Handle setting humidity threshold
    else if (String(topic) == setHumidityTopic){
        humidityThreshold = message.toFloat();
    }
    // Handle setting light intensity threshold
    else if (String(topic) == setIntensityTopic){
        lightThreshold = message.toInt();
    }
}

// Function to publish sensor data (temperature, humidity, soil moisture, light intensity)
void publishSensorData(){
    // Read data from DHT11
    float temperature = dht.readTemperature();  
    float humidity = dht.readHumidity();        

    // Read soil moisture data
    int rawSoilValue = analogRead(pinSoilSensor); 
    float soilMoisture = 100 - (rawSoilValue / float(4095)) * 100;  

    // Read light intensity data (assuming photoresistor)
    int lightValue = analogRead(pinLightSensor); 
    float lightIntensity = (lightValue / 1023.0) * 100;  

    // Publish sensor data to MQTT
    mqttClient.publish(mqttTemperatureTopic, String(temperature).c_str(), true);
    mqttClient.publish(mqttHumidityTopic, String(humidity).c_str(), true);
    mqttClient.publish(mqttSoilTopic, String(soilMoisture).c_str(), true);
    mqttClient.publish(mqttLightTopic, String(lightIntensity).c_str(), true);

    // Control devices based on sensor data
    if (isPumpAutoMode){
        controlPump(soilMoisture);
    }
    if (isLEDAutoMode){
        controlLED(lightValue);
    }
    if (isFanAutoMode){
        controlFan(temperature, humidity);
    }
}

// Function to control pump based on soil moisture
void controlPump(float soilMoisture){
    if (soilMoisture < soilThreshold){
        digitalWrite(pinPump, LOW);       // Turn pump ON
        publishDeviceStatus(pumpStatusTopic, true);
        pumpStartTime = millis();         // Record start time
    }
    else{
        digitalWrite(pinPump, HIGH);      // Turn pump OFF
        publishDeviceStatus(pumpStatusTopic, false);
        pumpActiveTime = (millis() - pumpStartTime) / 1000;     // Calculate active time in seconds
        publishDeviceActiveTime(pumpTimeTopic, pumpActiveTime); // Publish active time
    }
}

// Function to control LED based on light level
void controlLED(int lightLevel){
    if (lightLevel < lightThreshold){
        digitalWrite(pinLED, LOW);        // Turn LED ON
        publishDeviceStatus(ledStatusTopic, true);
        ledStartTime = millis();          // Record start time
    }
    else{
        digitalWrite(pinLED, HIGH);       // Turn LED OFF
        publishDeviceStatus(ledStatusTopic, false);
        ledActiveTime = (millis() - ledStartTime) / 1000; // Calculate active time in seconds
        publishDeviceActiveTime(ledTimeTopic, ledActiveTime); // Publish active time
    }
}

// Function to control fan based on temperature and humidity
void controlFan(float temperature, float humidity){
    if (temperature > temperatureThreshold || humidity > humidityThreshold){
        digitalWrite(pinFan, LOW); // Turn fan ON
        publishDeviceStatus(fanStatusTopic, true);
        fanStartTime = millis(); // Record start time
    }
    else{
        digitalWrite(pinFan, HIGH); // Turn fan OFF
        publishDeviceStatus(fanStatusTopic, false);
        fanActiveTime = (millis() - fanStartTime) / 1000; // Calculate active time in seconds
        publishDeviceActiveTime(fanTimeTopic, fanActiveTime); // Publish active time
    }
}

// Function to publish device status to MQTT
void publishDeviceStatus(const char *topic, bool status){
    mqttClient.publish(topic, status ? "ON" : "OFF", true);
}

// Function to publish device active time to MQTT
void publishDeviceActiveTime(const char *topic, unsigned long activeTime){
    mqttClient.publish(topic, String(activeTime).c_str(), true);
}

// Function to reconnect to MQTT
void reconnectMQTT(){
    while (!mqttClient.connected()){
        Serial.println("Reconnecting to MQTT...");
        String clientID = "ESP32-" + String(WiFi.macAddress());
        if (mqttClient.connect(clientID.c_str(), MQTT::username, MQTT::password, mqttLWTTopic, 0, false, mqttLWTMessage)){
            Serial.printf("Connected to MQTT as %s\n", clientID.c_str());
            mqttClient.subscribe(pumpModeTopic); 
            mqttClient.subscribe(pumpControlTopic); 
            mqttClient.subscribe(ledModeTopic);
            mqttClient.subscribe(ledControlTopic); 
            mqttClient.subscribe(fanModeTopic); 
            mqttClient.subscribe(fanControlTopic); 
            mqttClient.subscribe(setTemperatureTopic); 
            mqttClient.subscribe(setMoistureTopic);
            mqttClient.subscribe(setHumidityTopic); 
            mqttClient.subscribe(setIntensityTopic); 
        }
        else{
            Serial.printf("Failed to connect, MQTT state: %d. Retrying in 1 second...\n", mqttClient.state());
            delay(1000);
        }
    }
}