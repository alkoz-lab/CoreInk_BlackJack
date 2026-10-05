# Blackjack for M5Stack CoreInk

A single-player [Blackjack](https://en.wikipedia.org/wiki/Blackjack) game for the M5Stack CoreInk, built with Arduino and
M5Unified. The interface is designed for the CoreInk's 200 × 200 e-ink display and three-button rocker.

The in-game **RULES** screen describes this app's simplified variant, which is based around reaching the max score.

Watch the [YouTube demo](https://youtube.com/shorts/ukWiBB_fCVw?feature=share).

## App screenshots

![Splash screen](<docs/screenshots/01 Splash Screen.png>)

![Main menu](<docs/screenshots/02 Main Menu.png>) ![Play until score selection](<docs/screenshots/04 Menu Play Until.png>) ![Game, player's turn](<docs/screenshots/05 Game Players turn.png>) ![Game, round over](<docs/screenshots/06 Game round over.png>)

## Build and upload

### Install the development tools with winget

In PowerShell, install Arduino IDE:

```powershell
winget install --id ArduinoSA.IDE.stable --exact
```

After installation, open Arduino IDE and install the M5Stack ESP32 platform
through Boards Manager and **M5Unified** through Library Manager.

### Requirements

- M5Stack CoreInk board support for Arduino ESP32 (`m5stack:esp32`).
- The M5Unified Arduino library.
- Arduino IDE (which includes Arduino CLI).
- Visual Studio Build Tools with the C++ tools workload (for host tests).

Open `blackjack/blackjack.ino` as the sketch in Arduino IDE, select
**M5Stack CoreInk** and the connected serial port, then upload.

To compile from the project root with Arduino CLI:

```powershell
arduino-cli compile --build-path "$env:TEMP\bj-fw" --fqbn m5stack:esp32:m5stack_coreink .\blackjack
```

The short build path helps avoid Windows path-length problems in dependencies.
The firmware also expects the M5Unified library to be installed in the selected
Arduino environment.

## Run the tests

In PowerShell, install Visual Studio Build Tools with the C++ toolchain:

```powershell
winget install --id Microsoft.VisualStudio.2022.BuildTools --exact --override "--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
```

Allow the Visual Studio installer to elevate if prompted. The Build Tools
command installs the Visual C++ tools workload and its recommended components.

From the project root in PowerShell:

```powershell
.\tests\run-tests.ps1
```

The host tests are compiled with MSVC. The winget command above installs the
required C++ toolchain.

To run the host tests and compile the CoreInk firmware:

```powershell
.\tests\run-tests.ps1 -Firmware
```

The firmware option also requires Arduino IDE's `arduino-cli.exe` at the
location used by the test script.

## Project layout

```text
assets/                  Card bitmap data
blackjack/
  blackjack.ino          Arduino sketch entry point
  BlackjackApp.*         Screen flow and application composition
  Card, Hand, Deck.*     Cards and game domain
  RoundController.*      Round orchestration
  RoundJudge.*           Outcome calculation
  Score.*                Match scoring
  TableScreen.*          Game table presentation
  *Screen.*              Splash and game screens
  src/CoreInkKit/         Reusable display, input, battery, and power helpers
docs/screenshots/         Screenshots used in this README
tests/                   Host-side tests and hardware stubs
```

`CoreInkKit` remains inside this project under `blackjack/src/CoreInkKit`.
Game rules and round orchestration are kept separate from display and button
handling so the game logic can be tested on a PC.

## License

This project is licensed under the [MIT License](LICENSE).
