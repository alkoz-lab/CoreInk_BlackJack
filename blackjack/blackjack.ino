#include <M5Unified.h>
#include <exception>

#include "BlackjackGame.hpp"
#include "Blackjack.hpp"
#include "Presenter.hpp"

Presenter presenter;
BlackjackGame game(5, presenter);

void setup()
{
  M5.begin();
  M5.Display.setEpdMode(lgfx::epd_mode_t::epd_fast);
  M5.Display.setAutoDisplay(false);
  presenter.showSplash();
}

void loop()
{
  try
  {
    switch (presenter.showMenu())
    {
    case Presenter::MenuAction::StartGame:
      game.reset(presenter.chooseMaxScore());
      while (!game.isGameOver())
      {
        game.playRound();
      }
      presenter.gameOver(game.score());
      return;
    case Presenter::MenuAction::Rules:
      presenter.showRules(Blackjack::rules());
      return;
    case Presenter::MenuAction::PowerOff:
      presenter.showPowerOffMessage();
      break;
    }
  }
  catch (const std::exception &ex)
  {
     presenter.fatalError(ex.what());
  }

  // E-ink keeps the final screen while powered off.
  M5.Display.waitDisplay();
  delay(1000);
  M5.Power.powerOff();
}
