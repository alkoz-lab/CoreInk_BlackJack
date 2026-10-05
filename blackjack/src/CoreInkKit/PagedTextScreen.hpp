#pragma once
#include <M5Unified.h>
#include <cstddef>
#include "Buttons.hpp"
#include "PromptBar.hpp"

// Title "<title> page/pages" plus lines of text; rocker UP/DOWN pages, press returns.
// A line consisting of "\n" is drawn as an empty line.
class PagedTextScreen
{
public:
    static constexpr int TEXT_Y = 32;

    PagedTextScreen(M5GFX &display, Buttons &buttons, const PromptBar &prompt)
        : display_(display), buttons_(buttons), prompt_(prompt)
    {
    }

    void show(const char *title, const char *const *lines, std::size_t lineCount);

private:
    M5GFX &display_;
    Buttons &buttons_;
    const PromptBar &prompt_;

    void drawPage(const char *title, const char *const *lines, int lineCount, int page,
                  int pageCount, int linesPerPage);
};
