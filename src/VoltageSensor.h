#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>
#include "ResultCodes.h"

class VoltageSensor {
public:
    // Configuration structure
    struct Config {
        uint8_t adcPin = 2;  // ADC pin number
    };

public:
    // Constructor
    VoltageSensor(const Config& config);
    
    // Destructor
    ~VoltageSensor();
    
    // Core functionality
    HRESULT ReadRawADC(uint16_t& rawValue);     // Read raw ADC value, returns S_OK on success
    
    // Configuration access
    Config GetConfig() const;

private:
    Config m_config;
    bool m_initialized;
    
    // Helper methods
    void Initialize();                          // Initialize ADC (private, auto-called)
};

#endif // VOLTAGE_SENSOR_H