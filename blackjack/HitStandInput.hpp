#pragma once
#include <M5Unified.h>
#include "src/CoreInkKit/Buttons.hpp"
#include "src/CoreInkKit/EinkFrame.hpp"
#include "IPlayerInput.hpp"
#include "src/CoreInkKit/PromptBar.hpp"

// Asks Hit or Stand on the prompt line: rocker DOWN = Hit, UP = Stand.
class HitStandInput : public IPlayerInput
{
public:
    HitStandInput(M5GFX &display, Buttons &buttons, const PromptBar &prompt)
        : display_(display), buttons_(buttons), prompt_(prompt)
    {
    }

    Decision askHitOrStand() override
    {
        prompt_.draw(display_, "\x8F:Hit   \x8C:Stand");
        EinkFrame::present(display_);
        return buttons_.waitFor({Buttons::Button::Down, Buttons::Button::Up}) ==
                       Buttons::Button::Down
                   ? Decision::Hit
                   : Decision::Stand;
    }

private:
    M5GFX &display_;
    Buttons &buttons_;
    const PromptBar &prompt_;
};
