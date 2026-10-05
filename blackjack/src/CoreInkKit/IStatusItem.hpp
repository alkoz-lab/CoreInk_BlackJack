#pragma once
#include <M5Unified.h>

// Something drawn in a status bar's right-hand slot (e.g. the battery). Draw-only.
class IStatusItem
{
public:
    virtual ~IStatusItem() = default;
    // Draws right-aligned to `right` within the text line at `y`; returns the drawn left x.
    virtual int drawRightAligned(M5GFX &d, int right, int y, int foreground,
                                 int background) const = 0;
};
