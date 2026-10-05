#pragma once
#include <M5Unified.h>
#include "src/CoreInkKit/Buttons.hpp"
#include "src/CoreInkKit/PromptBar.hpp"

// Blackjack title art: a fanned hand of random cards under the title.
class SplashScreen
{
public:
    SplashScreen(M5GFX &display, Buttons &buttons, const PromptBar &prompt)
        : display_(display), buttons_(buttons), prompt_(prompt)
    {
    }

    // Startup splash, then waits for any button.
    void show();
    // Final frame kept by the e-ink while powered off.
    void showPowerOff();

private:
    M5GFX &display_;
    Buttons &buttons_;
    const PromptBar &prompt_;

    void drawArt();
};
