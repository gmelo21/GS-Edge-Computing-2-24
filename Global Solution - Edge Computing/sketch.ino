#include <WiFi.h>          // Include WiFi library
#include <PubSubClient.h>  // Include PubSubClient for MQTT
#include "DHTesp.h"        // Include DHT sensor library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Pin Configuration ---
const int DHT_PIN = 15;               // DHT sensor pin
const int VOLTAGE_SENSOR_PIN = 39;    // Voltage sensor pin
const int ACS712_PIN = 34;            // ACS712 current sensor pin
const int LUMINOSITY_SENSOR_PIN = 35; // Luminosity sensor pin
const int LED_PIN = 2;                // LED output pin

// --- WiFi and MQTT Configuration ---
const char* SSID = "Wokwi-GUEST";
const char* PASSWORD = "";
const char* BROKER_MQTT = "191.235.32.167";
const int BROKER_PORT = 1883;
const char* TOPIC_SUBSCRIBE = "/TEF/lampEDGE_GS/cmd";
const char* TOPIC_PUBLISH_STATE = "/TEF/lampEDGE_GS/attrs";
const char* TOPIC_PUBLISH_LUMINOSITY = "/TEF/lampEDGE_GS/attrs/l";
const char* TOPIC_PUBLISH_HUMIDITY = "/TEF/lampEDGE_GS/attrs/h";
const char* TOPIC_PUBLISH_TEMPERATURE = "/TEF/lampEDGE_GS/attrs/t";
const char* TOPIC_PUBLISH_VOLTAGE = "/TEF/lampEDGE_GS/attrs/v";
const char* TOPIC_PUBLISH_CURRENT = "/TEF/lampEDGE_GS/attrs/a";
const char* ID_MQTT = "fiware_EDGE_GS";

// --- LCD Setup ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Constants for Sensors ---
const float ADC_RESOLUTION = 4095.0; // ADC resolution (12-bit for ESP32)
const float REFERENCE_VOLTAGE = 3.3; // Reference voltage of the ESP32
const float ACS712_SENSITIVITY = 0.1; // Sensitivity in volts per ampere (100 mV/A)

// --- Objects and Variables ---
WiFiClient espClient;
PubSubClient MQTT(espClient);
DHTesp dhtSensor;
char outputState = '0';

// --- Logging ---
unsigned long lastLCDUpdate = 0;
const unsigned long lcdUpdateInterval = 2000; // 2 seconds
int lcdState = 0;

// --- Initialization Functions ---
void initSerial() {
    Serial.begin(115200);
}

void initWiFi() {
    delay(10);
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqttCallback);
}

void initOutput() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}

void initLCD() {
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    delay(2000);
    lcd.clear();
}

// --- MQTT Callback ---
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String msg = "";
    for (int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }
    Serial.print("Message received: ");
    Serial.println(msg);
    if (msg == "lampEDGE_GS@on|") {
        digitalWrite(LED_PIN, HIGH);
        outputState = '1';
    } else if (msg == "lampEDGE_GS@off|") {
        digitalWrite(LED_PIN, LOW);
        outputState = '0';
    }
}

// --- Helper Functions ---
float readVoltage() {
    int sensorValue = analogRead(VOLTAGE_SENSOR_PIN);
    return (sensorValue / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
}

float readCurrent() {
    int sensorValue = analogRead(ACS712_PIN);
    float voltage = (sensorValue / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
    return voltage / ACS712_SENSITIVITY;
}

int readLuminosity() {
    int sensorValue = analogRead(LUMINOSITY_SENSOR_PIN);
    return map(sensorValue, 0, 4095, 0, 100);
}

TempAndHumidity readDHTSensor() {
    return dhtSensor.getTempAndHumidity();
}

void publishToMQTT() {
    TempAndHumidity data = readDHTSensor();
    float voltage = readVoltage();
    float current = readCurrent();
    int luminosity = readLuminosity();

    String stateMsg = outputState == '1' ? "s|on" : "s|off";
    MQTT.publish(TOPIC_PUBLISH_STATE, stateMsg.c_str());
    MQTT.publish(TOPIC_PUBLISH_VOLTAGE, String(voltage, 2).c_str());
    MQTT.publish(TOPIC_PUBLISH_CURRENT, String(current, 2).c_str());
    MQTT.publish(TOPIC_PUBLISH_LUMINOSITY, String(luminosity).c_str());
    MQTT.publish(TOPIC_PUBLISH_HUMIDITY, String(data.humidity, 1).c_str());
    MQTT.publish(TOPIC_PUBLISH_TEMPERATURE, String(data.temperature, 1).c_str());
}

void displayToLCD() {
    if (millis() - lastLCDUpdate >= lcdUpdateInterval) {
        lastLCDUpdate = millis();
        lcd.clear();

        if (lcdState == 0) {
            // Display Voltage and Current
            float voltage = readVoltage();
            float current = readCurrent();
            lcd.setCursor(0, 0);
            lcd.print("Volt: ");
            lcd.print(voltage, 2);
            lcd.print("V");
            lcd.setCursor(0, 1);
            lcd.print("Amp: ");
            lcd.print(current, 2);
            lcd.print("A");
        } else if (lcdState == 1) {
            // Display Temperature and Humidity
            TempAndHumidity data = readDHTSensor();
            lcd.setCursor(0, 0);
            lcd.print("Temp: ");
            lcd.print(data.temperature, 1);
            lcd.print("C");
            lcd.setCursor(0, 1);
            lcd.print("Hum: ");
            lcd.print(data.humidity, 1);
            lcd.print("%");
        } else if (lcdState == 2) {
            // Display Luminosity
            int luminosity = readLuminosity();
            lcd.setCursor(0, 0);
            lcd.print("Luminosity: ");
            lcd.setCursor(0, 1);
            lcd.print(luminosity);
            lcd.print("%");
        }

        // Update state
        lcdState = (lcdState + 1) % 3;
    }
}

// --- Setup ---
void setup() {
    initSerial();
    initWiFi();
    initMQTT();
    initOutput();
    initLCD();
    dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
}

// --- Main Loop ---
void loop() {
    if (!MQTT.connected()) {
        while (!MQTT.connected()) {
            Serial.println("Reconnecting to MQTT...");
            if (MQTT.connect(ID_MQTT)) {
                Serial.println("Connected to MQTT");
                MQTT.subscribe(TOPIC_SUBSCRIBE);
            } else {
                Serial.println("Failed to connect to MQTT, retrying...");
                delay(2000);
            }
        }
    }

    publishToMQTT();
    displayToLCD();

    MQTT.loop();
    delay(1000);
}
