#pragma once
#include <M5Unified.h>
#include "IStatusItem.hpp"
#include "TextUtil.hpp"

// Inverse (white on black) line: text on the left, optional item (battery) on the right. Draw-only.
class StatusBar
{
public:
    explicit StatusBar(int y = 0, const IStatusItem *rightItem = nullptr)
        : y_(y), rightItem_(rightItem)
    {
    }

    void draw(M5GFX &d, const char *text) const
    {
        TextUtil::beginInverseLine(d, y_);
        TextUtil::drawAt(d, 0, y_, text);
        if (rightItem_)
            rightItem_->drawRightAligned(d, d.width(), y_, TFT_WHITE, TFT_BLACK);
        d.setTextColor(TFT_BLACK);
    }

private:
    int y_;
    const IStatusItem *rightItem_;
};
