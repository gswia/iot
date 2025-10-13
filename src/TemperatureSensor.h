#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ResultCodes.h"

class TemperatureSensor {
public:
    // Configuration structure
    struct Config {
        uint8_t oneWireBusPin = 4;  // GPIO pin for OneWire bus
    };

public:
    // Constructor
    TemperatureSensor(const Config& config);
    
    // Destructor
    ~TemperatureSensor();
    
    // Core functionality
    HRESULT ReadTemperature(float& tempC);      // Read temperature in Celsius, returns S_OK on success
    
    // Configuration access
    Config GetConfig() const;

private:
    Config m_config;
    OneWire* m_oneWire;
    DallasTemperature* m_sensors;
    bool m_initialized;
    
    // Helper methods
    void Initialize();                          // Initialize sensor hardware (sensors.begin())
    void Cleanup();
};

#endif // TEMPERATURE_SENSOR_H