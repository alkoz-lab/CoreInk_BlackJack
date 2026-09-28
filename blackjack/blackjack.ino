#include <M5Unified.h>
#include <exception>

#include "BlackjackGame.hpp"
#include "Presenter.hpp"

Presenter presenter;
BlackjackGame game(5, presenter);

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Display.setAutoDisplay(false);
}

void loop() {
  try {
    if (!game.isGameOver()) {
      game.playRound();
      return;
    }

    presenter.gameOver(game.score());
  }
  catch (const std::exception& ex) {
    Serial.printf("FATAL ERROR: %s\n", ex.what());
    presenter.fatalError(ex.what());
  }

  // E-ink keeps the final screen while powered off.
  M5.Display.waitDisplay();
  M5.Power.powerOff();
}
