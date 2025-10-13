#include "PowerManager.h"

PowerManager::PowerManager(const Config& config) : config_(config) {
}

void PowerManager::GoToSleep() {
    Serial.printf("+ PowerManager::GoToSleep\n");
    Serial.printf("Going to sleep for %d seconds\n", config_.sleepSeconds);
    Serial.flush();
    
    configureSleep(config_.sleepSeconds);
}

void PowerManager::configureSleep(uint32_t durationSeconds) {
    esp_sleep_enable_timer_wakeup(durationSeconds * 1000000);
    esp_deep_sleep_start();
}

PowerManager::Config PowerManager::GetConfig() const {
    return config_;
}