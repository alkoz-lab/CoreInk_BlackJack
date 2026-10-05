#pragma once
#include "src/CoreInkKit/BatteryIndicator.hpp"
#include "src/CoreInkKit/BatteryMonitor.hpp"
#include "src/CoreInkKit/Buttons.hpp"
#include "Deck.hpp"
#include "HitStandInput.hpp"
#include "src/CoreInkKit/MenuScreen.hpp"
#include "src/CoreInkKit/MessageScreen.hpp"
#include "src/CoreInkKit/PagedTextScreen.hpp"
#include "src/CoreInkKit/PromptBar.hpp"
#include "RoundController.hpp"
#include "SplashScreen.hpp"
#include "TableScreen.hpp"

// Composition root and top-level flow. Construct after M5.begin().
//
//   Splash ─▶ Menu ─┬─▶ Game ──▶ Menu
//                   ├─▶ Rules ─▶ Menu
//                   └─▶ PowerOff ─▶ Off      (any exception ─▶ fatal message ─▶ Off)
class BlackjackApp
{
public:
    enum class State
    {
        Splash,
        Menu,
        Game,
        Rules,
        PowerOff,
        Off
    };

    static constexpr int DEFAULT_MAX_SCORE = 5;

    explicit BlackjackApp(Buttons::IdleStep idle = nullptr);

    // Runs one screen and returns the next state. Never throws: errors show a message
    // and lead to Off.
    State step(State state);
    // Runs forever: steps through states and powers off at Off. On USB power the device
    // stays on, so it continues at the menu.
    [[noreturn]] void run();

private:
    BatteryMonitor battery_;
    BatteryIndicator batteryIndicator_;
    Buttons buttons_;
    PromptBar prompt_;
    SplashScreen splash_;
    MenuScreen menu_;
    PagedTextScreen pages_;
    MessageScreen messages_;
    TableScreen table_;
    HitStandInput input_;
    Deck deck_;
    RoundController game_;

    State runState(State state);
    State showMenu();
    State playGame();
};
