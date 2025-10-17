#include "TemperatureSensor.h"
#include "utils.h"

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
    
    // Read temperature 16 times and take median for stability
    Serial.print("Reading temperature...");
    const int numReadings = 16;
    float readings[numReadings];
    
    // Take multiple readings and store them
    for (int i = 0; i < numReadings; i++) {
        // Request temperature reading
        m_sensors->requestTemperatures();
        
        // Get temperature from first sensor
        float currentTemp = m_sensors->getTempCByIndex(0);
        
        // Check for sensor disconnection error on each reading
        if (currentTemp == DEVICE_DISCONNECTED_C) {
            Serial.println("Error: Could not read temperature from sensor");
            return E_SENSOR_DISCONNECTED;
        }
        
        readings[i] = currentTemp;
        delay(10); // Small delay between readings for sensor stability
    }
    
    // Calculate median
    tempC = calculateMedian(readings, numReadings);
    
    Serial.printf("%.2f°C (median of %d readings)\n", tempC, numReadings);
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