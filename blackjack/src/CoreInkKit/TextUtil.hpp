#pragma once
#include <M5Unified.h>

// Text drawing helpers shared by widgets and screens. Draw-only: never call display().
namespace TextUtil
{
    constexpr int LINE_HEIGHT = 16; // fonts::AsciiFont8x16

    inline void drawAt(M5GFX &d, int x, int y, const char *text)
    {
        d.setFont(&fonts::AsciiFont8x16);
        d.setCursor(x, y);
        d.print(text);
    }

    // Centers ASCII text horizontally within [x, x + width).
    inline void drawCentered(M5GFX &d, int x, int width, int y, const char *text)
    {
        d.setFont(&fonts::AsciiFont8x16);
        drawAt(d, x + (width - d.textWidth(text)) / 2, y, text);
    }

    // Screen title in Satisfy_24, centered at the top of the screen.
    inline void drawTitle(M5GFX &d, const char *title)
    {
        d.setFont(&fonts::Satisfy_24);
        d.setTextColor(TFT_BLACK);
        d.setTextDatum(textdatum_t::top_center);
        d.drawString(title, d.width() / 2, 0);
        d.setTextDatum(textdatum_t::top_left);
        d.setFont(&fonts::AsciiFont8x16);
    }

    // Fills a full-width black line and selects white ASCII text for drawing on it.
    inline void beginInverseLine(M5GFX &d, int y)
    {
        d.fillRect(0, y, d.width(), LINE_HEIGHT, TFT_BLACK);
        d.setTextColor(TFT_WHITE);
        d.setFont(&fonts::AsciiFont8x16);
    }
}
