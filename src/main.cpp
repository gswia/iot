#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_sleep.h"

// WiFi credentials - replace with your network details
const char* ssid = "Swiatki";
const char* password = "Lamik290875";

// API endpoint
const char* apiUrl = "https://stayproai-fa.azurewebsites.net/api/sensor/v1/challenge";

// Function declarations
void parseApiResponse(String response);
void callChallengeAPI();
void connectWiFi();

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
  int signalStrength = WiFi.RSSI();
  
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



void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) { // 10 second timeout
    delay(500);
    Serial.print(".");
    timeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.printf("IP: %s, Signal: %d dBm\n", 
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed, going to sleep...");
    Serial.flush();
    esp_sleep_enable_timer_wakeup(10 * 1000000); // 10 seconds
    esp_deep_sleep_start();
  }
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
  connectWiFi();
  
  // Call the challenge API
  callChallengeAPI();
  
  // Go to deep sleep for 30 seconds (increased for testing)
  Serial.println("========================================");
  Serial.println("Going to deep sleep for 30 seconds...");
  Serial.println("*** DISCONNECT AND RECONNECT SERIAL MONITOR NOW ***");
  Serial.println("========================================");
  Serial.flush(); // Ensure message is sent before sleep
  
  esp_sleep_enable_timer_wakeup(30 * 1000000); // 30 seconds in microseconds
  esp_deep_sleep_start();
}

void loop() {
  // This will never be executed - device resets on deep sleep wake
  // All logic is now in setup() which runs after each wake
}