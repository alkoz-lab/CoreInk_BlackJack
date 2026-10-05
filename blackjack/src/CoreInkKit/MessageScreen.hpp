#pragma once
#include <M5Unified.h>
#include "Buttons.hpp"
#include "PromptBar.hpp"

// Simple message screens: a titled message that waits for a button, and a fatal error.
class MessageScreen
{
public:
    static constexpr int FIRST_LINE_Y = 64;
    static constexpr int LINE_PITCH = 26; // Satisfy_24

    MessageScreen(M5GFX &display, Buttons &buttons, const PromptBar &prompt)
        : display_(display), buttons_(buttons), prompt_(prompt)
    {
    }

    // Title plus centered Satisfy_24 lines, then waits for any button.
    void show(const char *title, const char *const *lines, int lineCount, const char *prompt);
    // Error header plus wrapped message; returns immediately (the caller powers off).
    void showFatal(const char *message);

private:
    M5GFX &display_;
    Buttons &buttons_;
    const PromptBar &prompt_;
};
