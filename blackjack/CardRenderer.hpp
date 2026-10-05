#pragma once
#include <M5Unified.h>
#include "Card.hpp"

// The only code that knows the card bitmap assets and their indexing. Draw-only.
namespace CardRenderer
{
    constexpr int WIDTH = 56;
    constexpr int HEIGHT = 80;

    const unsigned char *faceBitmap(const Card &card); // falls back to the back for invalid cards
    const unsigned char *backBitmap();

    void drawFace(M5GFX &d, int x, int y, const Card &card);
    void drawBack(M5GFX &d, int x, int y);
}
