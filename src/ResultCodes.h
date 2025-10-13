#ifndef RESULT_CODES_H
#define RESULT_CODES_H

#include <cstdint>

// HRESULT-style result codes
// Success codes (>= 0)
constexpr int32_t S_OK = 0;                         // Operation succeeded

// WiFi error codes
constexpr int32_t E_WIFI_NOT_CONNECTED = -100;     // WiFi not connected

// Helper macros for result checking
#define SUCCEEDED(hr) ((hr) >= 0)
#define FAILED(hr) ((hr) < 0)

#endif // RESULT_CODES_H