#include "TableScreen.hpp"
#include "src/CoreInkKit/EinkFrame.hpp"
#include <cstdio>
#include <string>

namespace
{
    const char *resultText(const Hand &hand, Side side, Outcome outcome)
    {
        const Outcome sideWins = side == Side::Dealer ? Outcome::DealerWins : Outcome::PlayerWins;
        if (outcome == Outcome::Draw)
            return "DRAW!";
        if (outcome == sideWins)
            return "WINS!";
        if (hand.isBust())
            return "BUSTS!";
        return "";
    }
}

TableScreen::TableScreen(M5GFX &display, Buttons &buttons, const PromptBar &prompt,
                         const IStatusItem &battery)
    : display_(display), buttons_(buttons), prompt_(prompt), status_(STATUS_Y, &battery)
{
}

HandView TableScreen::handView(Side side) const
{
    const int cardAreaWidth = display_.width() - ANNOTATION_WIDTH;
    return side == Side::Player
               ? HandView({PLAYER_Y, cardAreaWidth, ANNOTATION_WIDTH, 0, cardAreaWidth})
               : HandView({DEALER_Y, 0, ANNOTATION_WIDTH, ANNOTATION_WIDTH, cardAreaWidth});
}

void TableScreen::drawStatus(const TableState &table) const
{
    char line[48];
    const std::string scoreText = table.score.toString();
    snprintf(line, sizeof(line), " #%d   Score %s", table.round, scoreText.c_str());
    status_.draw(display_, line);
}

void TableScreen::showInitialDeal(const TableState &table)
{
    EinkFrame::begin(display_);
    drawStatus(table);
    handView(Side::Dealer).draw(display_, table.dealer, true, false);
    handView(Side::Player).draw(display_, table.player, false, false);
    EinkFrame::present(display_);
    delay(STEP_DELAY_MS);
}

void TableScreen::showHit(const TableState &table, Side side)
{
    const bool dealerHit = side == Side::Dealer;
    for (bool hideNewestCard : {true, false})
    {
        EinkFrame::begin(display_);
        drawStatus(table);
        handView(Side::Dealer).draw(display_, table.dealer, !dealerHit,
                                    hideNewestCard && dealerHit);
        handView(Side::Player).draw(display_, table.player, false,
                                    hideNewestCard && !dealerHit);
        EinkFrame::present(display_);
        display_.waitDisplay();
        if (hideNewestCard)
            delay(CARD_BACK_DELAY_MS);
    }
    delay(STEP_DELAY_MS);
}

void TableScreen::showRoundResult(const TableState &table, Outcome outcome)
{
    EinkFrame::begin(display_);
    drawStatus(table);
    handView(Side::Dealer).draw(display_, table.dealer, false, false,
                                resultText(table.dealer, Side::Dealer, outcome));
    handView(Side::Player).draw(display_, table.player, false, false,
                                resultText(table.player, Side::Player, outcome));
    prompt_.draw(display_, "Press any button");
    EinkFrame::present(display_);
    buttons_.waitForAny();
}
