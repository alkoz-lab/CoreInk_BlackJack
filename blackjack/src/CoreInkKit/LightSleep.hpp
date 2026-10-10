#pragma once
#include <cstdint>

// CoreInk idle step for Buttons: light-sleeps the ESP32 until a button is pressed.
// While waiting for input the CPU otherwise spins in a polling loop at full clock;
// in light sleep RAM, GPIO levels (power hold, e-ink lines) and millis() are preserved.
// Device-only: uses ESP-IDF sleep APIs.
namespace LightSleep
{
    // Waits for the e-ink refresh to finish, then sleeps until any of the rocker (GPIO
    // 37/38/39) or top button (GPIO 5) reads low. If a button is already held it just
    // polls, so M5.update() can debounce the press.
    void untilButton();

    // Waits for the e-ink refresh to finish, then light-sleeps until the timer expires.
    void forDuration(uint32_t milliseconds);
}
