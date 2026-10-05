#pragma once
#include <M5Unified.h>

// Bottom inverse line with centered text. Draw-only: callers present the frame.
class PromptBar
{
public:
    static constexpr int DEFAULT_Y = 184;

    // Font8x8C64 button glyphs that may be mixed with ASCII in prompt text.
    static constexpr char GLYPH_UP = '\x8C';
    static constexpr char GLYPH_DOWN = '\x8F';
    static constexpr char GLYPH_SELECT = '\x8A';

    explicit PromptBar(int y = DEFAULT_Y) : y_(y) {}

    int y() const { return y_; }
    void draw(M5GFX &d, const char *text) const;
    void drawOutline(M5GFX &d) const;

private:
    int y_;
};
