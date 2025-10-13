#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <Arduino.h>
#include "ResultCodes.h"

class WiFiManager {
public:
    // Configuration structure
    struct Config {
        const char* ssid;
        const char* password;
        uint8_t maxRetries = 3;
        uint32_t retryDelayMs = 2000;
    };

public:
    // Constructor
    WiFiManager(const Config& config);
    
    // Core functionality
    bool Connect();                    // Returns true if connected after retries
    bool IsConnected() const;
    void Disconnect();
    
    // Network information
    int32_t GetSignalStrength(int8_t& rssi) const;  // Returns S_OK on success, E_NOT_CONNECTED on failure
    IPAddress GetLocalIP() const;
    String GetSSID() const;
    
    // Configuration access
    Config GetConfig() const;

private:
    Config config_;
    uint32_t connectionTimeoutMs_ = 10000;  // Fixed timeout per attempt
    
    // Internal methods
    bool performSingleConnection();
    void initializeWiFi();
};

#endif // WIFI_MANAGER_H