#include "CardRenderer.hpp"
#include "../assets/progmem.h"

namespace
{
    // Asset order: back, then for each rank A..K the suits clubs, diamonds, hearts, spades.
    int bitmapIndex(const Card &card)
    {
        if (card.rank() < Card::RANK_ACE || card.rank() > Card::RANK_KING)
            return 0;

        int suitOffset;
        if (card.suit() == Card::SUIT_CLUBS)
            suitOffset = 0;
        else if (card.suit() == Card::SUIT_DIAMONDS)
            suitOffset = 1;
        else if (card.suit() == Card::SUIT_HEARTS)
            suitOffset = 2;
        else if (card.suit() == Card::SUIT_SPADES)
            suitOffset = 3;
        else
            return 0;

        return 1 + (card.rank() - 1) * 4 + suitOffset;
    }

    void drawBitmap(M5GFX &d, int x, int y, const unsigned char *bitmap)
    {
        d.drawBitmap(x, y, bitmap, CardRenderer::WIDTH, CardRenderer::HEIGHT,
                     TFT_WHITE, TFT_BLACK);
    }
}

const unsigned char *CardRenderer::faceBitmap(const Card &card)
{
    return epd_bitmap_allArray[bitmapIndex(card)];
}

const unsigned char *CardRenderer::backBitmap()
{
    return epd_bitmap_00_back;
}

void CardRenderer::drawFace(M5GFX &d, int x, int y, const Card &card)
{
    drawBitmap(d, x, y, faceBitmap(card));
}

void CardRenderer::drawBack(M5GFX &d, int x, int y)
{
    drawBitmap(d, x, y, backBitmap());
}
