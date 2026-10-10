#include "../blackjack/BlackjackApp.hpp"
#include "../blackjack/src/CoreInkKit/Power.hpp"
#include "check.hpp"
#include <string>

namespace
{
    using State = BlackjackApp::State;

    bool lastFrameHas(const char *text)
    {
        for (const auto &line : M5.Display.frames.back().text)
            if (line == text)
                return true;
        return false;
    }

    // Buttons::waitFor() discards the first update, then checks the next one.
    void scriptPresses(std::initializer_list<TestPress> presses)
    {
        testPressScript.clear();
        for (TestPress press : presses)
        {
            testPressScript.push_back(PRESS_NONE);
            testPressScript.push_back(press);
        }
    }

    int idleCalls = 0;
    void pressSelectWhenIdle()
    {
        ++idleCalls;
        testPressed = PRESS_B;
    }

    void checkTransitions()
    {
        BlackjackApp app;
        require(app.step(State::Splash) == State::Menu, "splash leads to the menu");
        require(app.step(State::Rules) == State::Menu, "rules return to the menu");

        testPressed = PRESS_B;
        require(app.step(State::Menu) == State::Game, "first menu item starts a game");

        scriptPresses({PRESS_C, PRESS_B});
        require(app.step(State::Menu) == State::Rules, "second menu item opens the rules");

        scriptPresses({PRESS_C, PRESS_C, PRESS_B});
        require(app.step(State::Menu) == State::PowerOff, "third menu item powers off");

        scriptPresses({PRESS_A, PRESS_B});
        require(app.step(State::Menu) == State::PowerOff, "up from the first item wraps to the last");

        require(app.step(State::PowerOff) == State::Off, "power-off screen leads to Off");
        require(lastFrameHas("Blackjack"), "power-off screen is drawn");
        testPressed = PRESS_ALL;
    }

    void checkIdleStep()
    {
        Buttons buttons(pressSelectWhenIdle);
        testPressScript.clear();
        testPressed = PRESS_NONE;
        displayEvents.clear();
        require(buttons.waitForAny() == Buttons::Button::Select, "idle step can deliver the press");
        require(idleCalls == 1, "idle step runs while nothing is pressed");
        require(displayEvents.empty(), "a custom idle step replaces the polling delay");

        Buttons polling;
        testPressScript = {PRESS_NONE, PRESS_NONE, PRESS_B}; // discarded, idle once, pressed
        displayEvents.clear();
        polling.waitForAny();
        require(displayEvents.size() == 1 && displayEvents[0] == "delay:100",
                "default idle step polls every 100 ms");
        testPressed = PRESS_ALL;
    }

    void checkShutdown()
    {
        displayEvents.clear();
        Power::shutdown(M5.Display);
        require(displayEvents.size() == 4 && displayEvents[0] == "wait" &&
                    displayEvents[1] == "delay:1000" && displayEvents[2] == "powerOff" &&
                    displayEvents[3] == "wakeup",
                "shutdown finishes the refresh, settles, powers off, and wakes the display if still on");
    }
}

int main()
{
    checkTransitions();
    checkIdleStep();
    checkShutdown();
    std::puts("app_states: all checks passed");
    return 0;
}
