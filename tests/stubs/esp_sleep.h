#pragma once
#include <M5Unified.h>
#include <cstdint>
#include <string>
#include "esp_err.h"

enum esp_sleep_source_t
{
    ESP_SLEEP_WAKEUP_GPIO,
    ESP_SLEEP_WAKEUP_TIMER
};

inline esp_err_t esp_sleep_enable_gpio_wakeup() { return ESP_OK; }

inline esp_err_t esp_sleep_enable_timer_wakeup(uint64_t time_in_us)
{
    displayEvents.emplace_back("timer:" + std::to_string(time_in_us));
    return ESP_OK;
}

inline esp_err_t esp_light_sleep_start()
{
    displayEvents.emplace_back("lightSleep");
    return ESP_OK;
}

inline esp_err_t esp_sleep_disable_wakeup_source(esp_sleep_source_t source)
{
    displayEvents.emplace_back(source == ESP_SLEEP_WAKEUP_TIMER ? "disableTimer" : "disableGpio");
    return ESP_OK;
}
