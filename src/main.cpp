#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_sleep.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "WiFiManager.h"
#include "ResultCodes.h"
#include "PowerManager.h"

// WiFi configuration
WiFiManager::Config wifiConfig;
WiFiManager* wifiManager = nullptr;

// Power configuration
PowerManager::Config powerConfig;
PowerManager powerManager(powerConfig);

// API endpoint
const char* apiUrl = "https://stayproai-fa.azurewebsites.net/api/sensor/v1/challenge";

// DS18B20 temperature sensor setup
#define ONE_WIRE_BUS 4  // Try GPIO2 first, if issues try GPIO3 (D3)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

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
  // Initialize temperature sensor
  sensors.begin();
  
  // Get WiFi signal strength
  int8_t rssi;
  int signalStrength = -999; // Default error value
  if (wifiManager != nullptr) {
    HRESULT result = wifiManager->GetSignalStrength(rssi);
    if (SUCCEEDED(result)) {
      signalStrength = rssi;
    }
  }
  
  // Read temperature from DS18B20
  Serial.print("Reading temperature...");
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  
  if(temperatureC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature from sensor");
    temperatureC = -999.0; // Use error value
  } else {
    Serial.printf("%.2f°C\n", temperatureC);
  }
  
  // Create JSON payload using ArduinoJson
  JsonDocument payloadDoc;
  payloadDoc["deviceId"] = "chujka";
  payloadDoc["wifiSignalStrengthDbm"] = signalStrength;
  payloadDoc["temperatureCelsius"] = temperatureC;
  
  String jsonPayload;
  serializeJson(payloadDoc, jsonPayload);
  
  Serial.printf("Payload: %s (Signal: %d dBm, Temp: %.2f°C)\n", jsonPayload.c_str(), signalStrength, temperatureC);
  
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
  // Initialize serial communication at 115200 baud rate
  Serial.begin(115200);
  delay(1000); // Give serial monitor time to reconnect
  
  // Print multiple messages to ensure visibility
  Serial.println();
  Serial.println("========================================");
  
  // Check wake reason
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("*** WAKING UP FROM DEEP SLEEP ***");
  } else {
    Serial.println("*** STARTING UP (POWER-ON/RESET) ***");
  }
  
  Serial.printf("Wake reason: %d\n", wakeup_reason);
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.println("========================================");
  
  // Connect to WiFi
  HRESULT wifiResult = connectWiFi();
  
  if (SUCCEEDED(wifiResult)) {
    // Call the temperature API instead of challenge API
    callTemperatureAPI();
  }
  
  // Clean up WiFiManager before sleep
  if (wifiManager != nullptr) {
    delete wifiManager;
    wifiManager = nullptr;
  }
  
  powerManager.GoToSleep();
}

void loop() {
  // This will never be executed - device resets on deep sleep wake
  // All logic is now in setup() which runs after each wake
}