#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_sleep.h"
#include "WiFiManager.h"
#include "ResultCodes.h"
#include "PowerManager.h"
#include "TemperatureSensor.h"
#include "VoltageSensor.h"

// WiFi configuration
WiFiManager::Config wifiConfig;
WiFiManager* wifiManager = nullptr;

// Power configuration
PowerManager::Config powerConfig;
PowerManager powerManager(powerConfig);

// Temperature sensor configuration
TemperatureSensor::Config tempConfig;
TemperatureSensor* temperatureSensor = nullptr;

// Voltage sensor configuration
VoltageSensor::Config voltageConfig;
VoltageSensor* voltageSensor = nullptr;

// API endpoint
const char* apiUrl = "https://stayproai-fa.azurewebsites.net/api/sensor/v1/challenge";

// Function declarations
void parseApiResponse(String response);
void callChallengeAPI();
void callTemperatureAPI();
HRESULT connectWiFi();

void parseApiResponse(String response) {
  // Create a JSON document
  JsonDocument doc;
  
  // Parse the JSON response
  DeserializationError error = deserializeJson(doc, response);
  
  if (error) {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    return;
  }
  
  // Extract values using proper JSON parsing
  const char* deviceId = doc["DeviceId"];
  const char* challenge = doc["Challenge"];
  
  if (deviceId && challenge) {
    Serial.println("Parsed API Response:");
    Serial.print("DeviceId: ");
    Serial.println(deviceId);
    Serial.print("Challenge: ");
    Serial.println(challenge);
  } else {
    Serial.println("Error: Missing DeviceId or Challenge in JSON response");
  }
}

void callChallengeAPI() {
  // Get WiFi signal strength
  int8_t rssi;
  int signalStrength = -999; // Default error value
  if (wifiManager != nullptr) {
    HRESULT result = wifiManager->GetSignalStrength(rssi);
    if (SUCCEEDED(result)) {
      signalStrength = rssi;
    }
  }
  
  // Create JSON payload using ArduinoJson
  JsonDocument payloadDoc;
  payloadDoc["deviceId"] = "chujka";
  payloadDoc["wifiSignalStrengthDbm"] = signalStrength;
  
  String jsonPayload;
  serializeJson(payloadDoc, jsonPayload);
  
  Serial.printf("Payload: %s (Signal: %d dBm)\n", jsonPayload.c_str(), signalStrength);
  
  const int maxRetries = 3;
  bool success = false;
  
  for (int attempt = 1; attempt <= maxRetries && !success; attempt++) {
    Serial.printf("Calling challenge API (attempt %d/%d)...\n", attempt, maxRetries);
    
    HTTPClient http;
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(15000); // 15 second timeout
    
    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("API Response:");
      Serial.println(response);
      
      // Parse the JSON response
      parseApiResponse(response);
      success = true;
    } else {
      Serial.printf("Error on HTTP request (attempt %d): %d\n", attempt, httpResponseCode);
      
      if (attempt < maxRetries) {
        Serial.printf("Retrying in 2 seconds...\n");
        delay(2000); // Wait 2 seconds before retry
      }
    }
    
    http.end();
  }
  
  if (!success) {
    Serial.println("All retry attempts failed!");
  }
}

void callTemperatureAPI() {
  // Get WiFi signal strength
  int8_t rssi;
  int signalStrength = -999; // Default error value
  if (wifiManager != nullptr) {
    HRESULT result = wifiManager->GetSignalStrength(rssi);
    if (SUCCEEDED(result)) {
      signalStrength = rssi;
    }
  }
  
  // Read temperature from sensor
  float temperatureC;
  HRESULT tempResult = temperatureSensor->ReadTemperature(temperatureC);
  
  if (FAILED(tempResult)) {
    Serial.println("Error: Could not read temperature from sensor");
    temperatureC = -999.0; // Use error value
  }
  
  // Read battery voltage from ADC
  uint16_t batteryAdcVoltage;
  HRESULT voltageResult = voltageSensor->ReadRawADC(batteryAdcVoltage);
  
  if (FAILED(voltageResult)) {
    Serial.println("Error: Could not read voltage from sensor");
    batteryAdcVoltage = 0; // Use error value
  }
  
  // Create JSON payload using ArduinoJson
  JsonDocument payloadDoc;
  payloadDoc["deviceId"] = "chujka";
  payloadDoc["wifiSignalStrengthDbm"] = signalStrength;
  payloadDoc["temperatureCelsius"] = temperatureC;
  payloadDoc["batteryAdcVoltage"] = batteryAdcVoltage;
  
  String jsonPayload;
  serializeJson(payloadDoc, jsonPayload);
  
  Serial.printf("Payload: %s (Signal: %d dBm, Temp: %.2f°C, Voltage: %d mV)\n", jsonPayload.c_str(), signalStrength, temperatureC, batteryAdcVoltage);
  
  const int maxRetries = 3;
  bool success = false;
  const char* temperatureApiUrl = "https://stayproai-fa.azurewebsites.net/api/sensor/v1/temperature";
  
  for (int attempt = 1; attempt <= maxRetries && !success; attempt++) {
    Serial.printf("Calling temperature API (attempt %d/%d)...\n", attempt, maxRetries);
    
    HTTPClient http;
    http.begin(temperatureApiUrl);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(15000); // 15 second timeout
    
    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Temperature API Response:");
      Serial.println(response);
      success = true;
    } else {
      Serial.printf("Error on HTTP request (attempt %d): %d\n", attempt, httpResponseCode);
      
      if (attempt < maxRetries) {
        Serial.printf("Retrying in 2 seconds...\n");
        delay(2000); // Wait 2 seconds before retry
      }
    }
    
    http.end();
  }
  
  if (!success) {
    Serial.println("All temperature API attempts failed!");
  }
}

HRESULT connectWiFi() {
  // Initialize config
  wifiConfig.ssid = "Swiatki";
  wifiConfig.password = "Lamik290875";
  wifiConfig.maxRetries = 3;
  wifiConfig.retryDelayMs = 2000;
  
  // Create WiFiManager instance
  if (wifiManager == nullptr) {
    wifiManager = new WiFiManager(wifiConfig);
  }
  
  HRESULT connectResult = wifiManager->Connect();
  
  return connectResult;
}

void setup() {
  // Initialize power manager (includes serial setup, wake reason trace, and heap info)
  powerManager.Initialize();
  
  // Create temperature sensor
  tempConfig.oneWireBusPin = 4;
  temperatureSensor = new TemperatureSensor(tempConfig);
  
  // Create voltage sensor
  voltageConfig.adcPin = 2;
  voltageSensor = new VoltageSensor(voltageConfig);
  
  // Connect to WiFi
  HRESULT wifiResult = connectWiFi();
  
  if (SUCCEEDED(wifiResult)) {
    // Call the temperature API instead of challenge API
    callTemperatureAPI();
  }
  
  // Clean up managers before sleep
  if (wifiManager != nullptr) {
    delete wifiManager;
    wifiManager = nullptr;
  }
  
  if (temperatureSensor != nullptr) {
    delete temperatureSensor;
    temperatureSensor = nullptr;
  }
  
  if (voltageSensor != nullptr) {
    delete voltageSensor;
    voltageSensor = nullptr;
  }
  
  powerManager.GoToSleep();
}

void loop() {
  // This will never be executed - device resets on deep sleep wake
  // All logic is now in setup() which runs after each wake
}