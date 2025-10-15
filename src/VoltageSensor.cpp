#include "VoltageSensor.h"

VoltageSensor::VoltageSensor(const Config& config)
    : m_config(config)
    , m_initialized(false)
{
}

VoltageSensor::~VoltageSensor()
{
}

void VoltageSensor::Initialize()
{
    if (!m_initialized) {
        // Set ADC resolution to 12-bit (0-4095)
        analogReadResolution(12);
        m_initialized = true;
    }
}

HRESULT VoltageSensor::ReadRawADC(uint16_t& rawValue)
{
    // Initialize ADC if not already done
    Initialize();
    
    // Read ADC value 16 times and take average for stability
    Serial.print("Reading ADC...");
    const int numReadings = 16;
    long sum = 0;
    
    // Take multiple readings and sum them
    for (int i = 0; i < numReadings; i++) {
        sum += analogReadMilliVolts(m_config.adcPin);
        delay(1); // Small delay between readings
    }
    
    // Calculate average
    int averageReading = sum / numReadings;
    
    // Check for valid reading range (allow for ADC calibration variations)
    if (averageReading < 0 || averageReading > 4000) {
        Serial.println("Error: ADC reading out of range");
        return E_ADC_OUT_OF_RANGE;
    }
    
    rawValue = (uint16_t)averageReading;
    Serial.printf("%d mV (average of %d readings)\n", rawValue, numReadings);
    return S_OK;
}

VoltageSensor::Config VoltageSensor::GetConfig() const
{
    return m_config;
}