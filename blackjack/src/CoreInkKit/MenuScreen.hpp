#pragma once
#include <M5Unified.h>
#include "Buttons.hpp"
#include "PromptBar.hpp"

// Title plus a vertical list; rocker UP/DOWN moves the highlight, press selects.
class MenuScreen
{
public:
    MenuScreen(M5GFX &display, Buttons &buttons, const PromptBar &prompt)
        : display_(display), buttons_(buttons), prompt_(prompt)
    {
    }

    // Returns the index of the chosen item.
    int select(const char *title, const char *const *items, int itemCount);

private:
    M5GFX &display_;
    Buttons &buttons_;
    const PromptBar &prompt_;

    void draw(const char *title, const char *const *items, int itemCount, int selected);
};
