#include "PowerManager.h"

PowerManager::PowerManager(const Config& config) : config_(config) {
}

void PowerManager::GoToSleep() {
    Serial.printf("+ PowerManager::GoToSleep\n");
    Serial.printf("Going to sleep for %d seconds\n", config_.sleepSeconds);
    Serial.flush();
    
    configureSleep(config_.sleepSeconds);
}

void PowerManager::Initialize() {
    Serial.println("+ PowerManager::Initialize");

    // Initialize serial communication at 115200 baud rate
    Serial.begin(115200);
    delay(1000); // Give serial monitor time to reconnect
    
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println("*** WAKING UP FROM DEEP SLEEP ***");
    } else {
        Serial.println("*** STARTING UP (POWER-ON/RESET) ***");
    }
    
    Serial.printf("Wake reason: %d\n", wakeup_reason);
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());

    Serial.println("- PowerManager::Initialize");
}

void PowerManager::configureSleep(uint32_t durationSeconds) {
    esp_sleep_enable_timer_wakeup(durationSeconds * 1000000);
    esp_deep_sleep_start();
}

PowerManager::Config PowerManager::GetConfig() const {
    return config_;
}