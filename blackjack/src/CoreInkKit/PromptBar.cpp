#include "PromptBar.hpp"
#include "TextUtil.hpp"

namespace
{
    bool isButtonGlyph(char character)
    {
        return character == PromptBar::GLYPH_UP || character == PromptBar::GLYPH_DOWN ||
               character == PromptBar::GLYPH_SELECT;
    }
}

void PromptBar::draw(M5GFX &d, const char *text) const
{
    TextUtil::beginInverseLine(d, y_);

    const int asciiWidth = d.textWidth(" ");
    d.setFont(&fonts::Font8x8C64);
    const int glyphWidth = d.textWidth(" ");
    int textWidth = 0;
    for (const char *character = text; *character; ++character)
        textWidth += isButtonGlyph(*character) ? glyphWidth : asciiWidth;

    int x = textWidth < d.width() ? (d.width() - textWidth) / 2 : 0;
    for (const char *character = text; *character; ++character)
    {
        const bool glyph = isButtonGlyph(*character);
        if (glyph)
            d.setFont(&fonts::Font8x8C64);
        else
            d.setFont(&fonts::AsciiFont8x16);
        d.setCursor(x, y_ + (glyph ? 4 : 0));
        d.write(static_cast<uint8_t>(*character));
        x += glyph ? glyphWidth : asciiWidth;
    }
    d.setFont(&fonts::AsciiFont8x16);
    d.setTextColor(TFT_BLACK);
}

void PromptBar::drawOutline(M5GFX &d) const
{
    d.drawRect(0, y_, d.width(), TextUtil::LINE_HEIGHT, TFT_BLACK);
}
