#pragma once
#include <M5Unified.h>
#include <cstdint>
#include "src/CoreInkKit/IStatusItem.hpp"
#include "src/CoreInkKit/Buttons.hpp"
#include "HandView.hpp"
#include "ITableView.hpp"
#include "src/CoreInkKit/PromptBar.hpp"
#include "src/CoreInkKit/StatusBar.hpp"

// The game table: status bar (round, score, battery), dealer hand, player hand, prompt.
// Dealer: annotation left, cards right. Player: mirrored.
class TableScreen : public ITableView
{
public:
    static constexpr int STATUS_Y = 0;
    static constexpr int DEALER_Y = 18;
    static constexpr int PLAYER_Y = 101;
    static constexpr int ANNOTATION_WIDTH = 61;
    static constexpr uint32_t STEP_DELAY_MS = 200;
    static constexpr uint32_t CARD_BACK_DELAY_MS = 1000;

    TableScreen(M5GFX &display, Buttons &buttons, const PromptBar &prompt,
                const IStatusItem &battery);

    void showInitialDeal(const TableState &table) override;
    // Centers/compacts the hit hand, shows the new back for one second, then reveals it.
    void showHit(const TableState &table, Side side) override;
    void showRoundResult(const TableState &table, Outcome outcome) override;

    HandView handView(Side side) const;

private:
    M5GFX &display_;
    Buttons &buttons_;
    const PromptBar &prompt_;
    StatusBar status_;

    void drawStatus(const TableState &table) const;
};
