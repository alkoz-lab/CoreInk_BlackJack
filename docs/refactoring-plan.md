# Blackjack refactoring plan (draft for discussion)

Status: **Agreed.** Decisions are in §9; progress is tracked in §10.

## 1. Goals

1. Split the code into **reusable CoreInk components** (battery, status bar, prompt, menu, paged text, input, power) kept together in a `CoreInkKit` subfolder, so they could later be reused by other projects (not planned for now; see §9.10).
2. Apply **SOLID** so that game rules, round flow and e-ink rendering can change and be tested independently.
3. Show the battery in the game status line (originally every screen; narrowed after on-device review, see §9.9), without reading the ADC on every redraw.
4. Keep firmware behavior identical after each step: it must compile and the host tests must pass at every phase.

Non-goals: new game features, changing the visual design, or porting away from M5Unified.

## 2. Current state

| Unit | Responsibilities today | Problems |
|---|---|---|
| `Presenter` (~575 lines) | Splash, menu, max-score picker, rules pager, table, hand layout, card bitmaps, hit animation, status bar, **battery reading and drawing**, prompt bar, button waits, game over, power-off message, fatal error | **SRP**: about ten reasons to change. **OCP**: a new screen means editing this class. It uses the `M5` globals directly, so it can only be tested with stubs. It finds the player row by checking `y == PLAYER_Y` (hidden coupling). It mixes input with output (`askPlayerHitOrStand`, `selectMenu`). |
| `BlackjackGame` | Deal, player turn, dealer turn, judging, score, round counter | **DIP**: it depends on the concrete `Presenter`, including its UI vocabulary (`onePlayerTakesCards`, `anotherPlayerTakesCards`). It owns the dealer policy (hit below 17) inline. |
| `Blackjack` (static) | `determineWinner` and the rules text | Two unrelated things in one class: a pure rule, and a content resource that belongs to the UI. |
| `Deck` | Shuffling with a static `mt19937` seeded by `esp_random()` | The random source is hidden, so deals can't be made deterministic in tests. |
| `Score` | Tracks wins | Its `left`/`right` naming leaks the screen layout into the domain. |
| `blackjack.ino` | Wiring, menu dispatch, power off | Fine as a composition root, but it calls `M5.Power` directly. |

## 3. Should `blackjack.*` be merged into `blackjackGame.*`?

**Recommendation: no — split them by responsibility instead.**

- `determineWinner` is a **pure rule**: (dealer hand, player hand) → outcome. It's the easiest code to test and the most valuable to keep isolated. The double-bust bug was found and fixed there. Merging it into the stateful round controller would make it harder to test, not easier.
- `rules()` is **display content**. It belongs with the Rules screen, or as a text resource, not in the game logic.

Proposed result:

| New unit | Contents |
|---|---|
| `RoundJudge` (pure, header-only or `.cpp`) | `Outcome judge(const Hand& dealer, const Hand& player)` returns `DealerWins / PlayerWins / Draw`. It does **not** mutate `Score`; the caller applies the outcome (SRP). |
| `DealerPolicy` | `bool shouldHit(const Hand&)` (default: below 17). It's a strategy object, so a soft-17 variant can be added without editing the game (OCP). |
| `BlackjackGame` (renamed `RoundController`?) ❓ | Orchestrates one round, using the `Deck`, `RoundJudge` and `DealerPolicy` above plus the `ITableView` and `IPlayerInput` interfaces. |
| `rules_text.hpp` | `constexpr const char* RULES[]`, consumed by the Rules screen. |

## 4. Target architecture

```
 ┌──────────────── blackjack.ino (composition root) ────────────────┐
 │ creates platform objects, widgets, screens, game; runs app loop │
 └──────────────────────────────────────────────────────────────────┘
        │ uses                     │ uses                    │ uses
 ┌──────▼───────┐   depends on ┌───▼─────────────┐   ┌──────▼─────────┐
 │ Application  │──interfaces─▶│ Presentation     │   │ Platform       │
 │ RoundCtrl    │              │ (Blackjack-only) │   │ (CoreInkKit)   │
 └──────┬───────┘              │ TableScreen,     │   │ Buttons, Power,│
        │ uses                 │ HandView, Cards  │   │ BatteryMonitor │
 ┌──────▼───────┐              └───┬─────────────┘   │ EinkCanvas     │
 │ Domain       │                  │ uses             └──────▲─────────┘
 │ Card, Deck,  │              ┌───▼──────────────────────────┴───┐
 │ Hand, Score, │              │ Shared UI widgets (CoreInkKit)    │
 │ RoundJudge,  │              │ StatusBar, BatteryIndicator,      │
 │ DealerPolicy │              │ PromptBar, MenuScreen,            │
 └──────────────┘              │ PagedTextScreen, MessageScreen    │
                               └───────────────────────────────────┘
```

Dependency rule: arrows point **inward and downward only**.
- The domain layer includes nothing from M5.
- The application layer sees only interfaces.
- Only the platform layer touches the `M5` globals.

### 4.1 Domain (Blackjack-specific, pure C++, host-testable)

- `Card`, `Hand`: unchanged.
- `Deck(IRandom&)` or `Deck(uint32_t seed)` ❓: an injectable random source, which enables deterministic tests and replayable deals.
- `Score`: rename `left`/`right` to `dealer`/`player` ❓.
- `RoundJudge` and `DealerPolicy`, as in §3.

### 4.2 Application

```cpp
struct ITableView {                       // what the game needs to show
    virtual void showDeal(const TableState&) = 0;
    virtual void showHit(const TableState&, Side who) = 0;   // animation is the view's business
    virtual void showRoundResult(const TableState&, Outcome) = 0;
    virtual ~ITableView() = default;
};
struct IPlayerInput {
    enum class Decision { Hit, Stand };
    virtual Decision askHitOrStand() = 0;
    virtual ~IPlayerInput() = default;
};
struct TableState { int round; const Hand& dealer; const Hand& player;
                    const Score& score; bool dealerHoleHidden; };
```

- **ISP**: the game sees two small interfaces, not a 20-method presenter.
- **DIP**: `RoundController(Deck&, const DealerPolicy&, ITableView&, IPlayerInput&)`.
  Host tests can use a `FakeTableView` that records calls, and a scripted `FakePlayerInput`, so whole rounds can be tested without any M5 stubs.
- `Side { Dealer, Player }` replaces the `y == PLAYER_Y` check and the `revealDealerCards` flag.

### 4.3 Presentation (Blackjack-specific)

| Unit | Responsibility |
|---|---|
| `CardRenderer` | Draws a card face or back from `progmem.h`. It's the only code that knows the bitmap indexing. |
| `HandView` | Lays out one hand (centering, compaction, hidden hole card, hidden newest card) inside a `HandLayout { Rect cards; Rect annotation; }`. The dealer and the player are just two layouts, mirrored. |
| `HandAnnotation` | Draws the centered title, total and outcome text in the annotation rectangle. |
| `TableScreen : ITableView` | Composes the status bar, two `HandView`s, the annotations and the prompt. It owns the hit animation (back → 1 s → face) and implements the `ITableView` methods. |
| `HitStandInput : IPlayerInput` | Maps rocker DOWN/UP to Hit/Stand using `Buttons`. |
| Screens built from shared widgets | Splash (Blackjack art), the max-score picker (`MenuScreen`), Rules (`PagedTextScreen` with `RULES`), Game Over (`MessageScreen`). |

### 4.4 Shared, reusable components (CoreInkKit subfolder)

| Component | Responsibility | Notes |
|---|---|---|
| `BatteryMonitor` | Caches a battery percentage, and re-samples **only** when the cache is older than `interval` (lazy), or when `forceRefresh()` is called. Converts voltage to percent through a replaceable curve. | Sampling and drawing are separate classes (SRP). The voltage source is an injectable function, so it can be faked in tests. |
| `BatteryIndicator` | Draws the icon and percentage from `BatteryMonitor::percent()` into a given rectangle, with configurable colors. **It never calls `display()`.** | Uses the constants that already exist in `showStatus`, moved into a `BatteryIconStyle` struct. |
| `StatusBar` | Left text, optional right slot (`IStatusItem`, e.g. `BatteryIndicator`), inverse colors. | Only the game table passes the battery; the fatal-error header uses no right item. |
| `PromptBar` | Centered prompt text with button glyph handling. | |
| `MenuScreen` | Title, items, a cursor, and selection via the rocker. Returns the chosen index. | Used for the main menu and the max-score picker. |
| `PagedTextScreen` | Title, line wrapping or pages, and paging with the rocker. | Rules. |
| `MessageScreen` | Title and body, then waits for any button (game over, fatal error, power-off notice). | |
| `Buttons` | `update()`, `waitAny(timeout)`, and a `pressed(Button)` abstraction over `M5.BtnX`. It's the single place to add debounce and **light sleep while idle** ❓. | |
| `Power` | `powerOff(finalFrame)`: draws the last frame (with the battery), waits for the display, then powers off. Also an optional idle auto-off. | Replaces the duplicated `powerOff` code across projects. |
| `EinkFrame` / `Canvas` | `begin()`, then draw, then `present()` = one `display()` plus `waitDisplay()`. Enforces the "compose, then refresh once" rule from [coreink-battery-practices.md](../../shared/coreink-battery-practices.md). | Widgets draw onto a `LGFX_Device&`/`M5GFX&` passed in, and never refresh it themselves. |
| `text` helpers | `drawCentered(rect, text, font)`, `fitText`, glyph substitution. | `TextUtil.hpp`; glyph handling lives in `PromptBar`. |

**OCP in practice:** new screens are built by composing widgets, not by adding methods to one big class.
**LSP:** any `ITableView` or `IPlayerInput` implementation (real or fake) must honor the same contract. For example, `showHit` returns only after the newest card is face-up.

## 5. Battery indicator design (the agreed first component)

```cpp
class BatteryMonitor {
public:
    using VoltageSource = int (*)();                 // millivolts; default wraps M5.Power
    explicit BatteryMonitor(uint32_t intervalMs = 60'000, VoltageSource src = defaultSource);
    int  percent();                                  // lazy: re-samples only if stale
    void forceRefresh();                             // e.g. on wake from sleep
private:
    int cached_ = -1; uint32_t sampledAt_ = 0; ...
};
class BatteryIndicator {
public:
    BatteryIndicator(BatteryMonitor&, BatteryIconStyle = {});
    void draw(M5GFX& gfx, const Rect& area, uint16_t fg, uint16_t bg) const;  // never refreshes
    static int width(const BatteryIconStyle&);       // lets layouts reserve space
};
```

- The **first** call samples immediately, so a cold start never shows "--".
- Power Off screen: call `forceRefresh()` and draw the indicator, so the frozen e-ink image shows an accurate final level.
- Optionally average 4 ADC readings taken at one sample time to reduce noise (cheap).
- The voltage-to-percent curve (currently linear from 3.3 V to 4.15 V) is a replaceable function. A piecewise LiPo curve can be added later.

## 6. Shared code location

`CoreInkKit` stays **inside this project** until further notice (§9.10): `blackjack/src/CoreInkKit/`.

- The Arduino build compiles a sketch's `src/` subfolder recursively, so no library install, junction or extra compile flag is needed, and the Arduino IDE builds it as is.
- The sketch includes the kit as `#include "src/CoreInkKit/Buttons.hpp"`; files inside the kit include each other by plain name.
- Kit files must not include anything from the Blackjack code, so the folder can later be lifted out unchanged into an Arduino library (`library.properties` + `src/`) if other projects need it.
## 7. Phased migration (each phase: compile firmware + run host tests)

| Phase | Work | Risk |
|---|---|---|
| 0 | Add a script (`tests/run-tests.ps1`) that builds and runs all host tests with MSVC, plus a firmware compile check. | none |
| 1 | **BatteryMonitor + BatteryIndicator** inside Blackjack, then show them on every screen, including Power Off. Add a host test for the caching interval. | low |
| 2 | Extract `RoundJudge` and `DealerPolicy`. Move the rules text out of `Blackjack`. Optionally do the `Score` rename. Existing `determine_winner` tests are moved over to `RoundJudge`. | low |
| 3 | Introduce `ITableView` and `IPlayerInput`. `Presenter` implements them temporarily (an adapter), and `BlackjackGame` now depends only on the interfaces. Add a full-round test with fakes. | medium |
| 4 | Break up `Presenter`: text helpers → `StatusBar`/`PromptBar` → `CardRenderer`/`HandView` → `TableScreen` → `MenuScreen`/`PagedTextScreen`/`MessageScreen`. **`Presenter` is deleted at the end.** | medium |
| 5 | Add `Buttons` and `Power`, and make the `.ino` a thin composition root with an app-state loop. | low |
| 6 | Move the generic widgets and platform classes into the `CoreInkKit` subfolder (§6). | low |

Moving code (phases 1–6) is kept separate from changing behavior, so screenshots and gameplay should stay identical. The only intended visible change is the battery in the game status line.

## 8. Testing strategy

- **Domain and application:** pure C++ tests with no stubs (`RoundJudge`, `DealerPolicy`, `Deck` with a seed, `RoundController` with fakes).
- **Widgets:** keep the current `tests/stubs/M5Unified.h` approach. The stub display records draw calls, and tests assert on layout (centering, compaction, battery rectangle).
- **BatteryMonitor:** a fake voltage source plus an injectable clock, to verify that no re-sample happens before the interval has passed.
- A single `run-tests.ps1` builds everything, so VS Code can run it as one task.

## 9. Decisions (agreed 2026-10-05)

1. **Shared code:** a `CoreInkKit` subfolder of this project (§6, revised by §9.10).
2. **Naming:** `BlackjackGame` → `RoundController`. File names use **PascalCase**, matching their class names and `#include`s. The only exception is `blackjack.ino`, because Arduino requires the sketch file to match its folder name.
3. **Battery:** re-sample at most every **60 s**. Each sample averages 4 ADC reads, because M5Unified's `getBatteryVoltage()` on CoreInk makes a single `analogReadMilliVolts` call.
4. **Light sleep:** only if it gives a noticeable gain. Estimate from the ESP32 datasheet: while waiting for a button, the CPU sits in the `delay(20)` polling loop at 240 MHz, drawing roughly **20–50 mA**. Light sleep draws about **0.8 mA** for the ESP32, plus whatever the CoreInk board draws when idle. Players spend most of their time waiting for input, so this looks like an order-of-magnitude gain while idle. → Implement in phase 5 (`Buttons::waitAny` with GPIO wake on the rocker, GPIO37/38/39, and the power button), then **measure** with a USB power meter before keeping it.
5. **`Score`:** the domain API uses **dealer/player** names (`dealerWins()`, `playerScore()`, …), which makes the code clearer. The status-line text (`"+1:0"`) is unchanged, so no screen space is lost.
6. **Deck randomness:** for maximum variety, `std::shuffle` takes its random bits directly from the ESP32 **hardware RNG** (`esp_random()`) on every swap. Seeding `mt19937` with one 32-bit value could only produce 2³² of the 52! (≈2²²⁶) possible orders. The source is an injectable function pointer, so tests can use deterministic numbers. Cards are dealt through an `ICardSource` interface, so round tests can use a stacked deck.
7. **Exceptions:** **keep them.** The ESP32 Arduino core is always compiled with `-fexceptions` (see `esp32-libs/3.3.9/flags/cpp_flags`), so the exception runtime is linked in regardless. Exceptions cost nothing at runtime unless thrown. They are used only for impossible states (an empty deck). `-fno-rtti` is also set, so the code must not use `dynamic_cast` or `typeid`.
8. **Phases 1–3** are implemented together (see §10).
9. **Battery placement (revised after on-device review):** the battery is shown **only** in the game screen's status line, which has room reserved for it. Titles, menus, rules, game over, fatal error and the power-off frame do not show it. `StatusBar` takes an optional `IStatusItem` for its right-hand slot, so screens without a battery do not depend on `BatteryMonitor` at all.
10. **CoreInkKit scope (2026-10-05):** only separate the generic code into a `CoreInkKit` subfolder of this project. It stays with Blackjack until further notice; adopting it in other projects is dropped from the plan.
11. **Hit/Stand buttons (2026-10-05):** rocker DOWN = Hit, UP = Stand. The prompt and the rules text say the same.
12. **Auto-stand on 21 (2026-10-05):** a player total of 21 (dealt or after a hit) cannot improve, so `RoundController` stands without asking, saving a button press. The rules text mentions it. This is the one deliberate gameplay change during the refactor.

## 10. Progress

| Phase | Status |
|---|---|
| 0 Test runner script | ✅ `tests/run-tests.ps1` |
| 1 BatteryMonitor + BatteryIndicator (battery on the game status line only) | ✅ |
| 2 RoundJudge, DealerPolicy, RulesText, Score dealer/player | ✅ |
| 3 ITableView / IPlayerInput / ICardSource, RoundController | ✅ |
| 4 Break up Presenter (deleted) | ✅ see §11 |
| 5 Power, app-state loop, light sleep in `Buttons` (+ measurement) | ✅ code; ⏳ on-device check and measurement, see §12 |
| 6 Move generic parts to the CoreInkKit subfolder | ✅ see §13 |

## 11. Phase 4 result

`Presenter` is deleted. Its responsibilities now live in:

| Unit | Kind | Notes |
|---|---|---|
| `TextUtil.hpp` | generic | `drawAt`, `drawCentered`, `drawTitle` (Satisfy_24), `beginInverseLine`. |
| `EinkFrame.hpp` | generic | `begin()` clears the frame, `present()` refreshes once. |
| `StatusBar.hpp` + `IStatusItem.hpp` | generic | Inverse line; optional right item. `BatteryIndicator` implements `IStatusItem`. |
| `PromptBar` | generic | Centered prompt with button glyphs; draw-only. |
| `Buttons` | generic | `waitFor({…})` / `waitForAny()`. Pulled forward from phase 5 because every screen needs it; phase 5 adds light sleep here. |
| `MenuScreen`, `PagedTextScreen`, `MessageScreen` | generic | Main menu and max-score picker; Rules; Game over and fatal error. |
| `CardRenderer` | Blackjack | The only unit that includes `progmem.h`. |
| `HandView` | Blackjack | Cards plus annotation for one hand (`HandAnnotation` was folded in: it is three centered lines). |
| `TableScreen : ITableView` | Blackjack | Status bar with battery, both hands, hit animation, round result. |
| `HitStandInput : IPlayerInput` | Blackjack | Rocker DOWN = Hit, UP = Stand (swapped after on-device review, §9.11). |
| `SplashScreen` | Blackjack | Startup splash and power-off frame. |

`blackjack.ino` builds everything in a `BlackjackApp` struct inside `setup()` (after `M5.begin()`) and keeps the menu flow. Tests: `table_screen.cpp` (layout, hit animation, battery caching, results, input) and `screens.cpp` (no battery outside the table, no ADC reads there).

## 12. Phase 5 result

| Unit | Kind | Notes |
|---|---|---|
| `Buttons` | generic | Takes an optional idle step (`Buttons::IdleStep`, a plain function pointer). The default is `delay(20)` polling. |
| `LightSleep` | generic, device-only | `untilButton()`: waits for the e-ink refresh, then light-sleeps until GPIO 37/38/39 (rocker) or GPIO 5 (top button) goes low. If a button is already held it polls instead, so `M5.update()` can debounce the press. The power button (GPIO 27) is not a wake source. |
| `Power.hpp` | generic | `Power::shutdown(display)`: finish the refresh, settle 1 s, `M5.Power.powerOff()`. On USB power the CoreInk stays on, so the display is woken and the app continues at the menu. |
| `BlackjackApp` | Blackjack | Composition root plus state machine: Splash → Menu → Game/Rules → Menu; Menu → PowerOff → Off. `step()` turns any exception into the fatal message and Off. `run()` never returns. |

`blackjack.ino` is now only `setup()` (M5 init, e-ink mode, `static BlackjackApp app(...)`, `app.run()`) and an empty `loop()`. Test: `app_states.cpp` covers transitions, the idle hook and the shutdown order. The stub buttons can now be scripted (`testPressed`, `testPressScript`).

### Verify on the device
1. Every button (rocker up/press/down, top) wakes the device from a menu and from the Hit/Stand prompt, with no missed or doubled presses.
2. The device stays powered on battery while light-sleeping. Power hold on GPIO 12 relies on output levels being kept during light sleep.
3. POWER OFF still switches the device off.

### Measuring light sleep (decision §9.4)
1. Measure battery current in series with the battery (on USB the device also charges, which skews a USB meter).
2. Leave it on the main menu for a minute and note the average current.
3. Set `IDLE_LIGHT_SLEEP = false` in `blackjack.ino`, flash, and repeat.
4. Keep light sleep only if the saving is noticeable (expected: tens of mA while polling vs a few mA asleep).

## 13. Phase 6 result

The generic code now lives in `blackjack/src/CoreInkKit/` (moved with `git mv`, so history is kept):

| Group | Files |
|---|---|
| Battery | `BatteryMonitor`, `BatteryIndicator.hpp`, `IStatusItem.hpp` |
| Drawing | `EinkFrame.hpp`, `TextUtil.hpp`, `StatusBar.hpp`, `PromptBar` |
| Screens | `MenuScreen`, `PagedTextScreen`, `MessageScreen` |
| Platform | `Buttons`, `LightSleep`, `Power.hpp`, `RandomSource.hpp` |

- Blackjack files include the kit as `"src/CoreInkKit/<Name>.hpp"`; kit files include each other by plain name.
- `tests/run-tests.ps1` finds each suite's units in either folder, and fails if any kit file includes a non-kit file, which keeps the kit independent of Blackjack.
- What stays in `blackjack/`: the domain (`Card`, `Hand`, `Deck`, `Score`, `RoundJudge`, `DealerPolicy`, `Outcome`, `RulesText`), the application (`RoundController` and its interfaces), the Blackjack screens (`CardRenderer`, `HandView`, `TableScreen`, `HitStandInput`, `SplashScreen`) and the composition root (`BlackjackApp`, `blackjack.ino`).
- To reuse the kit elsewhere later, copy or link the folder into the other sketch's `src/`, or turn it into an Arduino library by adding `library.properties` (§6).
