#pragma once
#include <cstdint>
#include <initializer_list>

// Blocking button waits over the CoreInk rocker (A = up, B = press, C = down) and the
// top EXT button. Between polls it runs an idle step: by default a short delay, or a
// light sleep that wakes on a button (see LightSleep.hpp).
class Buttons
{
public:
    enum class Button
    {
        Up,
        Select,
        Down,
        Ext
    };

    // Called while waiting and nothing accepted was pressed; must return within a few
    // tens of milliseconds of any button press. nullptr = delay(POLL_INTERVAL_MS).
    using IdleStep = void (*)();
    static constexpr uint32_t POLL_INTERVAL_MS = 100; // 100ms = 10 times per second

    explicit Buttons(IdleStep idle = nullptr) : idle_(idle) {}

    // Ignores presses made earlier (e.g. during an e-ink refresh), then blocks until one of
    // `accepted` is pressed. Simultaneous presses resolve in the order given.
    Button waitFor(std::initializer_list<Button> accepted);
    Button waitForAny();

private:
    IdleStep idle_;
};
