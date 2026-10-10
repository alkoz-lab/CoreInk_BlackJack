#pragma once
#include "esp_err.h"

using gpio_num_t = int;
enum gpio_int_type_t
{
    GPIO_INTR_LOW_LEVEL
};

constexpr gpio_num_t GPIO_NUM_37 = 37;
constexpr gpio_num_t GPIO_NUM_38 = 38;
constexpr gpio_num_t GPIO_NUM_39 = 39;
constexpr gpio_num_t GPIO_NUM_5 = 5;

inline int gpio_get_level(gpio_num_t) { return 1; }
inline esp_err_t gpio_wakeup_enable(gpio_num_t, gpio_int_type_t) { return ESP_OK; }
inline esp_err_t gpio_wakeup_disable(gpio_num_t) { return ESP_OK; }
