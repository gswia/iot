#ifndef RESULT_CODES_H
#define RESULT_CODES_H

#include <cstdint>

// HRESULT type definition (following Windows COM conventions)
typedef int32_t HRESULT;

// HRESULT-style result codes
// Success codes (>= 0)
constexpr HRESULT S_OK = 0;                         // Operation succeeded

// WiFi error codes
constexpr HRESULT E_WIFI_NOT_CONNECTED = -100;     // WiFi not connected

// Temperature sensor error codes
constexpr HRESULT E_SENSOR_NOT_INITIALIZED = -200; // Sensor not properly initialized
constexpr HRESULT E_SENSOR_DISCONNECTED = -201;    // Sensor is disconnected or not responding

// Voltage sensor error codes
constexpr HRESULT E_ADC_OUT_OF_RANGE = -300;       // ADC value out of expected range

// Helper macros for result checking
#define SUCCEEDED(hr) ((HRESULT)(hr) >= 0)
#define FAILED(hr) ((HRESULT)(hr) < 0)

#endif // RESULT_CODES_H