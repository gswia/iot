#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(const Config& config)
    : m_config(config)
    , m_oneWire(nullptr)
    , m_sensors(nullptr)
    , m_initialized(false)
{
    // Create OneWire instance
    m_oneWire = new OneWire(m_config.oneWireBusPin);
    
    // Create DallasTemperature instance
    m_sensors = new DallasTemperature(m_oneWire);
}

TemperatureSensor::~TemperatureSensor()
{
    Cleanup();
}

void TemperatureSensor::Initialize()
{
    if (m_sensors != nullptr && !m_initialized) {
        m_sensors->begin();
        m_initialized = true;
    }
}

HRESULT TemperatureSensor::ReadTemperature(float& tempC)
{
    if (m_sensors == nullptr) {
        return E_SENSOR_NOT_INITIALIZED;
    }
    
    // Initialize sensor if not already done
    Initialize();
    
    // Request temperature reading
    Serial.print("Reading temperature...");
    m_sensors->requestTemperatures();
    
    // Get temperature from first sensor
    tempC = m_sensors->getTempCByIndex(0);
    
    // Check for sensor disconnection error
    if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: Could not read temperature from sensor");
        return E_SENSOR_DISCONNECTED;
    }
    
    Serial.printf("%.2f°C\n", tempC);
    return S_OK;
}

TemperatureSensor::Config TemperatureSensor::GetConfig() const
{
    return m_config;
}

void TemperatureSensor::Cleanup()
{
    if (m_sensors != nullptr) {
        delete m_sensors;
        m_sensors = nullptr;
    }
    
    if (m_oneWire != nullptr) {
        delete m_oneWire;
        m_oneWire = nullptr;
    }
}