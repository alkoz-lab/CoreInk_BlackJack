#pragma once
#include <M5Unified.h>

// "Compose, then refresh once": begin() clears the buffer, widgets draw, present() refreshes.
namespace EinkFrame
{
    inline void begin(M5GFX &d)
    {
        d.setEpdMode(epd_mode_t::epd_fast);
        d.fillScreen(TFT_WHITE);
        d.setTextColor(TFT_BLACK);
        d.setTextWrap(false, false);
        d.setTextDatum(textdatum_t::top_left);
        d.setFont(&fonts::AsciiFont8x16);
    }

    inline void present(M5GFX &d)
    {
        d.display();
    }
}
