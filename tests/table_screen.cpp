#include "../blackjack/src/CoreInkKit/BatteryIndicator.hpp"
#include "../blackjack/CardRenderer.hpp"
#include "../blackjack/HitStandInput.hpp"
#include "../blackjack/TableScreen.hpp"
#include "check.hpp"
#include <algorithm>
#include <cstdlib>

namespace
{
    constexpr int CARD_WIDTH = CardRenderer::WIDTH;
    constexpr int DEALER_Y = TableScreen::DEALER_Y;
    constexpr int PLAYER_Y = TableScreen::PLAYER_Y;
    constexpr int ANNOTATION_WIDTH = TableScreen::ANNOTATION_WIDTH;

    BatteryMonitor battery([] { return M5.Power.getBatteryVoltage(); }, [] { return millis(); });
    BatteryIndicator batteryIndicator(battery);
    Buttons buttons;
    PromptBar prompt;
    TableScreen table(M5.Display, buttons, prompt, batteryIndicator);

    bool hasText(const Frame &frame, const char *text)
    {
        return std::find(frame.text.begin(), frame.text.end(), text) != frame.text.end();
    }

    Hand makeHand(const char *name, int count)
    {
        Hand hand(name);
        for (int index = 0; index < count; ++index)
            hand.takeCard(Card(Card::SUIT_CLUBS, 2));
        return hand;
    }

    std::vector<BitmapDraw> rowCards(const Frame &frame, int y)
    {
        std::vector<BitmapDraw> cards;
        for (const auto &card : frame.cards)
            if (card.y == y)
                cards.push_back(card);
        return cards;
    }

    void checkRow(const Frame &frame, int y, int count)
    {
        const auto cards = rowCards(frame, y);
        require(static_cast<int>(cards.size()) == count, "all cards are drawn");
        const int areaX = y == PLAYER_Y ? 0 : ANNOTATION_WIDTH;
        const int areaRight = areaX + M5.Display.width() - ANNOTATION_WIDTH;
        const int leftMargin = cards.front().x - areaX;
        const int rightMargin = areaRight - cards.back().x - CARD_WIDTH;
        require(leftMargin >= 0 && rightMargin >= 0, "cards stay within their area");
        require(std::abs(leftMargin - rightMargin) <= 1, "hand is horizontally centered");
        if (count > 1)
        {
            const int expectedStep = std::min(CARD_WIDTH + 1,
                (areaRight - areaX - CARD_WIDTH) / (count - 1));
            for (int index = 1; index < count; ++index)
                require(cards[index].x - cards[index - 1].x == expectedStep,
                        "cards compact only as needed");
        }
    }

    void checkHit(bool dealerHit, int count)
    {
        M5.Display = {};
        displayEvents.clear();
        const Score score(5);
        const Hand dealer = makeHand("DEALER", dealerHit ? count : 2);
        const Hand player = makeHand("PLAYER", dealerHit ? 2 : count);
        table.showHit({1, dealer, player, score}, dealerHit ? Side::Dealer : Side::Player);
        require(M5.Display.frames.size() == 2, "hit has back and face frames");
        const std::vector<std::string> expectedEvents = {
            "display", "wait", "delay:1000", "display", "wait", "delay:200"};
        require(displayEvents == expectedEvents, "back stays visible for a second after refresh");
        const int hitY = dealerHit ? DEALER_Y : PLAYER_Y;
        const auto &backFrame = M5.Display.frames[0];
        const auto &faceFrame = M5.Display.frames[1];
        checkRow(backFrame, DEALER_Y, static_cast<int>(dealer.size()));
        checkRow(backFrame, PLAYER_Y, static_cast<int>(player.size()));
        const auto backCards = rowCards(backFrame, hitY);
        const auto faceCards = rowCards(faceFrame, hitY);
        for (int index = 0; index < count; ++index)
        {
            require(backCards[index].x == faceCards[index].x, "flip preserves positions");
            require(faceCards[index].bitmap == CardRenderer::faceBitmap(Card(Card::SUIT_CLUBS, 2)),
                    "hit cards finish face-up");
            require(backCards[index].bitmap == (index == count - 1 ?
                CardRenderer::backBitmap() : faceCards[index].bitmap),
                "only the newest hit card starts face-down");
        }
        if (!dealerHit)
        {
            require(rowCards(backFrame, DEALER_Y)[1].bitmap == CardRenderer::backBitmap() &&
                    rowCards(faceFrame, DEALER_Y)[1].bitmap == CardRenderer::backBitmap(),
                    "dealer hole card stays hidden during player hits");
        }
        const Hand &hitHand = dealerHit ? dealer : player;
        Hand previous(hitHand.name());
        for (int index = 0; index < count - 1; ++index)
            previous.takeCard(hitHand.getCard(index));
        char total[24];
        std::snprintf(total, sizeof(total), "ã=%d+?", previous.getValue());
        require(hasText(backFrame, total), "back frame does not reveal the new total");
        std::snprintf(total, sizeof(total), "ã=%d", hitHand.getValue());
        require(hasText(faceFrame, total), "face frame updates the total");
    }

    void checkBatteryOnStatusBar()
    {
        const auto lastFrameShows = [](const char *text) {
            return !M5.Display.frames.empty() && hasText(M5.Display.frames.back(), text);
        };
        const Hand dealer = makeHand("DEALER", 2);
        const Hand player = makeHand("PLAYER", 3);
        const Score score(5);

        M5.Power.millivolts = 4000;
        battery.forceRefresh();
        const int readsBeforeHit = M5.Power.reads;
        M5.Display = {};
        table.showHit({1, dealer, player, score}, Side::Player);
        require(lastFrameShows("82%"), "status bar shows the battery");
        require(M5.Power.reads == readsBeforeHit, "redraws reuse the cached battery level");

        testMillis += BatteryMonitor::DEFAULT_INTERVAL_MS;
        M5.Power.millivolts = 3725;
        M5.Display = {};
        table.showInitialDeal({1, dealer, player, score});
        require(lastFrameShows("50%") &&
                    M5.Power.reads == readsBeforeHit + BatteryMonitor::READS_PER_SAMPLE,
                "a stale battery level is re-sampled once");
    }

    void checkBatteryFill()
    {
        constexpr int full = BatteryIndicator::INNER_HEIGHT;
        static_assert(BatteryIndicator::fillHeightFor(0) == 0, "empty battery has no fill");
        static_assert(BatteryIndicator::fillHeightFor(100) == full, "full battery is filled");
        static_assert(BatteryIndicator::fillHeightFor(150) == full, "fill is clamped above");
        static_assert(BatteryIndicator::fillHeightFor(-5) == 0, "fill is clamped below");
        for (int percent = 1; percent <= 100; ++percent)
            require(BatteryIndicator::fillHeightFor(percent) >=
                        BatteryIndicator::fillHeightFor(percent - 1),
                    "fill grows with the charge level");
    }

    void checkRoundResultText()
    {
        const Hand dealer = makeHand("DEALER", 11); // 22: bust
        const Hand player = makeHand("PLAYER", 2);
        Score score(5);
        score.record(Outcome::PlayerWins);
        M5.Display = {};
        table.showRoundResult({1, dealer, player, score}, Outcome::PlayerWins);
        require(hasText(M5.Display.frames.back(), "WINS!") &&
                    hasText(M5.Display.frames.back(), "BUSTS!"),
                "winner and bust are annotated");
        M5.Display = {};
        table.showRoundResult({1, dealer, player, score}, Outcome::Draw);
        require(hasText(M5.Display.frames.back(), "DRAW!") &&
                    !hasText(M5.Display.frames.back(), "WINS!"),
                "draw is annotated on both hands");
    }

    void checkHitStandInput()
    {
        HitStandInput input(M5.Display, buttons, prompt);
        M5.Display = {};
        // waitFor() discards the first update, then reads the press.
        testPressScript = {PRESS_NONE, PRESS_C};
        require(input.askHitOrStand() == IPlayerInput::Decision::Hit, "rocker DOWN means Hit");
        require(M5.Display.frames.size() == 1, "the prompt costs exactly one refresh");
        testPressScript = {PRESS_NONE, PRESS_A};
        require(input.askHitOrStand() == IPlayerInput::Decision::Stand, "rocker UP means Stand");
        testPressScript = {PRESS_NONE, PRESS_B, PRESS_C};
        require(input.askHitOrStand() == IPlayerInput::Decision::Hit,
                "Select is ignored at the Hit/Stand prompt");
        testPressed = PRESS_ALL;
    }
}

int main()
{
    const Score score(5);
    for (int count = 1; count <= 21; ++count)
    {
        M5.Display = {};
        const Hand dealer = makeHand("DEALER", count);
        const Hand player = makeHand("PLAYER", count);
        table.showInitialDeal({1, dealer, player, score});
        checkRow(M5.Display.frames.back(), DEALER_Y, count);
        checkRow(M5.Display.frames.back(), PLAYER_Y, count);
    }
    for (int count = 3; count <= 21; ++count)
    {
        checkHit(false, count);
        checkHit(true, count);
    }
    checkBatteryOnStatusBar();
    checkBatteryFill();
    checkRoundResultText();
    checkHitStandInput();
    std::puts("All table layout, hit-animation and battery tests passed.");
}
