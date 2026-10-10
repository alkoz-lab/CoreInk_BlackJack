#include <M5Unified.h>
#include "Buttons.hpp"

namespace
{
    bool wasPressed(Buttons::Button button)
    {
        switch (button)
        {
        case Buttons::Button::Up:
            return M5.BtnA.wasPressed();
        case Buttons::Button::Select:
            return M5.BtnB.wasPressed();
        case Buttons::Button::Down:
            return M5.BtnC.wasPressed();
        case Buttons::Button::Ext:
            return M5.BtnEXT.wasPressed();
        }
        return false;
    }
}

Buttons::Button Buttons::waitFor(std::initializer_list<Button> accepted)
{
    M5.update();
    for(;;)
    {
        M5.update();
        for (Button button : accepted)
        {
            if (wasPressed(button))
                return button;
        }
        if (idle_)
            idle_();
        else
            delay(POLL_INTERVAL_MS);
    }
}

Buttons::Button Buttons::waitForAny()
{
    return waitFor({Button::Select, Button::Up, Button::Down, Button::Ext});
}
