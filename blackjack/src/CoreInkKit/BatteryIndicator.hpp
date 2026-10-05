#pragma once
#include <M5Unified.h>
#include <cstdio>
#include "BatteryMonitor.hpp"
#include "IStatusItem.hpp"

// Draws a small battery icon followed by "NN%". Draw-only: never calls display().
class BatteryIndicator : public IStatusItem
{
public:
    // Tune the icon here; derived values follow automatically.
    static constexpr int LINE_HEIGHT = 16; // fonts::AsciiFont8x16
    static constexpr int ICON_WIDTH = 6;
    static constexpr int ICON_HEIGHT = LINE_HEIGHT - 4;
    static constexpr int POLE_HEIGHT = 1;
    static constexpr int POLE_WIDTH = ICON_WIDTH / 2;
    static constexpr int BORDER_WIDTH = 1;
    static constexpr int ICON_TEXT_GAP = 3;

    static constexpr int BODY_HEIGHT = ICON_HEIGHT - POLE_HEIGHT;
    static constexpr int INNER_WIDTH = ICON_WIDTH - 2 * BORDER_WIDTH;
    static constexpr int INNER_HEIGHT = BODY_HEIGHT - 2 * BORDER_WIDTH;
    static constexpr int POLE_OFFSET_X = (ICON_WIDTH - POLE_WIDTH) / 2;
    static constexpr int ICON_OFFSET_Y = (LINE_HEIGHT - ICON_HEIGHT) / 2;

    static_assert(ICON_WIDTH > 2 * BORDER_WIDTH && BODY_HEIGHT > 2 * BORDER_WIDTH &&
                      BORDER_WIDTH > 0,
                  "Battery body must have a positive border and interior");
    static_assert(ICON_HEIGHT <= LINE_HEIGHT && POLE_HEIGHT > 0 &&
                      POLE_WIDTH > 0 && POLE_WIDTH <= ICON_WIDTH,
                  "Battery and pole must fit within one text line");

    explicit BatteryIndicator(BatteryMonitor &monitor) : monitor_(monitor) {}

    // Draws right-aligned to `right` within the text line starting at `y`.
    // Returns the left x of the drawn indicator so callers can lay out around it.
    int drawRightAligned(M5GFX &d, int right, int y, int foreground, int background) const override
    {
        const int percent = monitor_.percent();
        char text[8];
        snprintf(text, sizeof(text), "%d%%", percent);
        d.setFont(&fonts::AsciiFont8x16);
        const int textX = right - d.textWidth(text);
        const int iconX = textX - ICON_TEXT_GAP - ICON_WIDTH;
        const int iconY = y + ICON_OFFSET_Y;
        const int bodyY = iconY + POLE_HEIGHT;
        const int innerX = iconX + BORDER_WIDTH;
        const int innerY = bodyY + BORDER_WIDTH;

        d.fillRect(iconX, bodyY, ICON_WIDTH, BODY_HEIGHT, foreground);
        d.fillRect(innerX, innerY, INNER_WIDTH, INNER_HEIGHT, background);
        d.fillRect(iconX + POLE_OFFSET_X, iconY, POLE_WIDTH, POLE_HEIGHT, foreground);
        const int fillHeight = fillHeightFor(percent);
        if (fillHeight > 0)
            d.fillRect(innerX, innerY + INNER_HEIGHT - fillHeight, INNER_WIDTH, fillHeight,
                       foreground);
        d.setTextColor(foreground);
        d.setCursor(textX, y);
        d.print(text);
        return iconX;
    }

    // Filled interior rows for a charge level, rounded to the nearest row (clamped to 0..100%).
    static constexpr int fillHeightFor(int percent)
    {
        return (INNER_HEIGHT * (percent < 0 ? 0 : percent > 100 ? 100 : percent) + 50) / 100;
    }

private:
    BatteryMonitor &monitor_;
};
