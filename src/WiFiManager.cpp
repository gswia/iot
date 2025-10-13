#include "WiFiManager.h"

WiFiManager::WiFiManager(const Config& config) : config_(config) {
    initializeWiFi();
}

bool WiFiManager::Connect() {
    Serial.printf("WiFi connecting to '%s'", config_.ssid);
    
    for (uint8_t attempt = 1; attempt <= config_.maxRetries; attempt++) {
        Serial.printf(" (attempt %d/%d)", attempt, config_.maxRetries);
        
        if (performSingleConnection()) {
            Serial.println(" - Success!");
            int8_t signal;
            if (GetSignalStrength(signal) == S_OK) {
                Serial.printf("Connected! IP: %s, Signal: %d dBm\n", 
                             GetLocalIP().toString().c_str(), signal);
            } else {
                Serial.printf("Connected! IP: %s\n", GetLocalIP().toString().c_str());
            }
            return true;
        }
        
        Serial.printf(" - Failed");
        if (attempt < config_.maxRetries) {
            Serial.printf(", retrying in %dms", config_.retryDelayMs);
            delay(config_.retryDelayMs);
        }
    }
    
    Serial.println(" - All attempts failed!");
    return false;
}

bool WiFiManager::IsConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::Disconnect() {
    if (IsConnected()) {
        Serial.println("Disconnecting WiFi...");
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
    }
}

int32_t WiFiManager::GetSignalStrength(int8_t& rssi) const {
    if (IsConnected()) {
        rssi = WiFi.RSSI();
        return S_OK;
    }
    return E_WIFI_NOT_CONNECTED;
}

IPAddress WiFiManager::GetLocalIP() const {
    return WiFi.localIP();
}

String WiFiManager::GetSSID() const {
    return WiFi.SSID();
}

WiFiManager::Config WiFiManager::GetConfig() const {
    return config_;
}

bool WiFiManager::performSingleConnection() {
    WiFi.begin(config_.ssid, config_.password);
    
    uint32_t startTime = millis();
    while (WiFi.status() != WL_CONNECTED && 
           (millis() - startTime) < connectionTimeoutMs_) {
        delay(500);
        Serial.print(".");
    }
    
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::initializeWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
}