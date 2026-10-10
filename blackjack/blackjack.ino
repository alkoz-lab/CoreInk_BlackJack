#include <M5Unified.h>
#include "BlackjackApp.hpp"
#include "src/CoreInkKit/LightSleep.hpp"

// Light-sleep between button presses. Set to false to compare battery draw.
constexpr bool IDLE_LIGHT_SLEEP = true;

void setup()
{
  M5.begin();
  M5.Display.setEpdMode(lgfx::epd_mode_t::epd_fast);
  M5.Display.setAutoDisplay(false);
  static BlackjackApp app(IDLE_LIGHT_SLEEP ? LightSleep::untilButton : nullptr);
  app.run();
}

void loop()
{
}