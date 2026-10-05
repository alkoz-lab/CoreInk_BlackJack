#include <M5Unified.h>
#include <driver/gpio.h>
#include <esp_sleep.h>
#include "Buttons.hpp"
#include "LightSleep.hpp"

namespace
{
    // Active-low CoreInk buttons: rocker up, press, down, and the top button.
    constexpr gpio_num_t WAKE_PINS[] = {GPIO_NUM_37, GPIO_NUM_38, GPIO_NUM_39, GPIO_NUM_5};

    bool anyButtonHeld()
    {
        for (gpio_num_t pin : WAKE_PINS)
        {
            if (gpio_get_level(pin) == 0)
                return true;
        }
        return false;
    }
}

void LightSleep::untilButton()
{
    // Never sleep mid-refresh: the e-ink transfer needs the CPU and SPI clocks.
    M5.Display.waitDisplay();
    if (anyButtonHeld())
    {
        delay(Buttons::POLL_INTERVAL_MS);
        return;
    }

    for (gpio_num_t pin : WAKE_PINS)
        gpio_wakeup_enable(pin, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();
    esp_light_sleep_start();
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_GPIO);
    for (gpio_num_t pin : WAKE_PINS)
        gpio_wakeup_disable(pin);
}
