#include <M5Unified.h>
#include "BlackjackApp.hpp"
#include "src/CoreInkKit/Power.hpp"
#include "RulesText.hpp"
#include <cstdio>
#include <exception>

namespace
{
    enum MainMenuItem
    {
        MENU_START_GAME,
        MENU_RULES,
        MENU_POWER_OFF,
        MENU_ITEM_COUNT
    };
    const char *const MAIN_MENU_ITEMS[MENU_ITEM_COUNT] = {"START GAME", "RULES", "POWER OFF"};
    const char *const MAX_SCORE_ITEMS[] = {"5", "10", "25", "50"};
    const int MAX_SCORES[] = {5, 10, 25, 50};
    constexpr int MAX_SCORE_COUNT = sizeof(MAX_SCORES) / sizeof(MAX_SCORES[0]);

    int readBatteryMillivolts() { return static_cast<int>(M5.Power.getBatteryVoltage()); }
    uint32_t readMillis() { return static_cast<uint32_t>(millis()); }
}

BlackjackApp::BlackjackApp(Buttons::IdleStep idle)
    : battery_(readBatteryMillivolts, readMillis),
      batteryIndicator_(battery_),
      buttons_(idle),
      prompt_(),
      splash_(M5.Display, buttons_, prompt_),
      menu_(M5.Display, buttons_, prompt_),
      pages_(M5.Display, buttons_, prompt_),
      messages_(M5.Display, buttons_, prompt_),
      table_(M5.Display, buttons_, prompt_, batteryIndicator_),
      input_(M5.Display, buttons_, prompt_),
      deck_(),
      game_(DEFAULT_MAX_SCORE, deck_, table_, input_)
{
}

BlackjackApp::State BlackjackApp::step(State state)
{
    try
    {
        return runState(state);
    }
    catch (const std::exception &ex)
    {
        messages_.showFatal(ex.what());
        return State::Off;
    }
}

void BlackjackApp::run()
{
    State state = State::Splash;
    while (true)
    {
        state = step(state);
        if (state == State::Off)
        {
            Power::shutdown(M5.Display);
            state = State::Menu;
        }
    }
}

BlackjackApp::State BlackjackApp::runState(State state)
{
    switch (state)
    {
    case State::Splash:
        splash_.show();
        return State::Menu;
    case State::Menu:
        return showMenu();
    case State::Game:
        return playGame();
    case State::Rules:
        pages_.show("Rules", RulesText::LINES, RulesText::LINE_COUNT);
        return State::Menu;
    case State::PowerOff:
        splash_.showPowerOff();
        return State::Off;
    case State::Off:
        break;
    }
    return State::Off;
}

BlackjackApp::State BlackjackApp::showMenu()
{
    switch (menu_.select("Blackjack", MAIN_MENU_ITEMS, MENU_ITEM_COUNT))
    {
    case MENU_START_GAME:
        return State::Game;
    case MENU_RULES:
        return State::Rules;
    default:
        return State::PowerOff;
    }
}

BlackjackApp::State BlackjackApp::playGame()
{
    game_.reset(MAX_SCORES[menu_.select("Play until", MAX_SCORE_ITEMS, MAX_SCORE_COUNT)]);
    while (!game_.isGameOver())
        game_.playRound();

    const Score &score = game_.score();
    char scoreLine[32];
    snprintf(scoreLine, sizeof(scoreLine), "Score %d:%d", score.dealerScore(), score.playerScore());
    const char *const lines[] = {
        score.dealerScore() > score.playerScore() ? "Dealer won." : "You won!", scoreLine};
    messages_.show("Game over", lines, 2, "Press any button");
    return State::Menu;
}
