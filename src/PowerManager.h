#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>
#include "esp_sleep.h"

class PowerManager {
public:
    // Configuration structure
    struct Config {
        uint32_t sleepSeconds = 30;
    };

public:
    // Constructor
    PowerManager(const Config& config);
    
    // Core sleep functionality
    void GoToSleep(); // Uses duration from config
    
    // Initialization and wake up tracing
    void Initialize();
    
    // Configuration access
    Config GetConfig() const;
    
private:
    Config config_;
    
    // Internal helper for sleep configuration
    void configureSleep(uint32_t durationSeconds);
};

#endif // POWER_MANAGER_H