#pragma once
#include <M5Unified.h>
#include <cstdint>

// Powering the CoreInk down while the e-ink keeps showing the last frame.
namespace Power
{
    constexpr uint32_t SETTLE_MS = 1000;

    // Lets the final frame finish refreshing, then cuts power. On USB power the CoreInk
    // cannot switch itself off, so this returns; the display is woken again so the
    // caller can carry on drawing.
    inline void shutdown(M5GFX &d)
    {
        d.waitDisplay();
        delay(SETTLE_MS);
        M5.Power.powerOff();
        d.wakeup();
    }
}
