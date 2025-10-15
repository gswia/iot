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
    
    // Read ADC value
    Serial.print("Reading ADC...");
    int adcReading = analogRead(m_config.adcPin);
    
    // Check for valid reading range (0-4095 for 12-bit ADC)
    if (adcReading < 0 || adcReading > 4095) {
        Serial.println("Error: ADC reading out of range");
        return E_ADC_OUT_OF_RANGE;
    }
    
    rawValue = (uint16_t)adcReading;
    Serial.printf("%d\n", rawValue);
    return S_OK;
}

VoltageSensor::Config VoltageSensor::GetConfig() const
{
    return m_config;
}