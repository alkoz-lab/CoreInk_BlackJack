#include <M5Unified.h>
#include "Presenter.hpp"
#include "Card.hpp"
#include "../assets/progmem.h"
#include <cstdio>
#include <random>
#include <esp_random.h>

namespace
{
    constexpr int LINE_H = 16; // ASCII font line height
    constexpr int ROUND_Y = 0;
    constexpr int SCORE_Y = 16;
    constexpr int DEALER_Y = 18;
    constexpr int PLAYER_Y = 101;
    constexpr int PROMPT_Y = 184;
    constexpr int CARD_X = 61;
    constexpr int CARD_WIDTH = 56;
    constexpr int CARD_HEIGHT = 80;
    constexpr int ANNOTATION_DY = 16;

    constexpr uint32_t STEP_DELAY_MS = 200;

    void beginInverseLine(int y)
    {
        auto &d = M5.Display;
        d.fillRect(0, y, d.width(), LINE_H, TFT_BLACK);
        d.setTextColor(TFT_WHITE);
        d.setFont(&fonts::AsciiFont8x16);
    }

    void drawText(int x, int y, const char *text)
    {
        auto &d = M5.Display;
        d.setFont(&fonts::AsciiFont8x16);
        d.setCursor(x, y);
        d.print(text);
    }

    int batteryPercent()
    {
        const float voltage = M5.Power.getBatteryVoltage() / 1000.0f;
        if (voltage < 3.3f)
            return 0;
        if (voltage > 4.15f)
            return 100;
        return static_cast<int>((voltage - 3.3f) * 100.0f / (4.15f - 3.3f));
    }

    void drawHandTotal(int y, int value, bool dealerHidden)
    {
        auto &d = M5.Display;
        d.setFont(&fonts::AsciiFont8x16);
        d.setCursor(0, y);
        d.print("ã=");
        d.print(value);
        if (dealerHidden)
        {
            d.print("+?");
        }
    }

    int cardBitmapIndex(const Card &card)
    {
        if (card.rank() < Card::RANK_ACE || card.rank() > Card::RANK_KING)
        {
            return 0;
        }

        int suitOffset;
        if (card.suit() == Card::SUIT_CLUBS)
            suitOffset = 0;
        else if (card.suit() == Card::SUIT_DIAMONDS)
            suitOffset = 1;
        else if (card.suit() == Card::SUIT_HEARTS)
            suitOffset = 2;
        else if (card.suit() == Card::SUIT_SPADES)
            suitOffset = 3;
        else
            return 0;

        return 1 + (card.rank() - 1) * 4 + suitOffset;
    }

    int randomSplashCardIndex(int firstRank, int lastRank)
    {
        static std::mt19937 generator(esp_random());
        const char suits[] = {Card::SUIT_CLUBS, Card::SUIT_DIAMONDS,
                              Card::SUIT_HEARTS, Card::SUIT_SPADES};
        std::uniform_int_distribution<int> rank(firstRank, lastRank);
        std::uniform_int_distribution<int> suit(0, 3);
        return cardBitmapIndex(Card(suits[suit(generator)], rank(generator)));
    }

    bool isButtonGlyph(uint8_t character)
    {
        return character == 0x8C || character == 0x8F || character == 0x8A;
    }

    int cardStepForHand(std::size_t cardCount, int displayWidth)
    {
        if (cardCount < 2)
        {
            return CARD_WIDTH + 1;
        }

        const int availableWidth = displayWidth - CARD_X;
        const int maximumStep = (availableWidth - CARD_WIDTH) /
                                static_cast<int>(cardCount - 1);
        const int naturalStep = CARD_WIDTH + 1;
        return maximumStep < naturalStep ? maximumStep : naturalStep;
    }

} // namespace

void Presenter::clearUp() const
{
    auto &d = M5.Display;
    d.setEpdMode(epd_mode_t::epd_quality);
    d.setEpdMode(epd_mode_t::epd_fast);
    d.fillScreen(TFT_WHITE);
    d.setTextColor(TFT_BLACK);
    d.setTextWrap(false, false);
    d.setTextDatum(textdatum_t::top_left);
    d.setFont(&fonts::AsciiFont8x16);
}

void Presenter::present() const
{
    M5.Display.display();
}

void Presenter::drawSplash() const
{
    auto &d = M5.Display;
    clearUp();
    d.drawBitmap(8, 34, epd_bitmap_00_back,
                 CARD_WIDTH, CARD_HEIGHT, TFT_WHITE, TFT_BLACK);
    d.drawBitmap(39, 49, epd_bitmap_allArray[randomSplashCardIndex(10, 10)],
                 CARD_WIDTH, CARD_HEIGHT, TFT_WHITE, TFT_BLACK);
    d.drawBitmap(86, 75, epd_bitmap_allArray[randomSplashCardIndex(Card::RANK_ACE, Card::RANK_ACE)],
                 CARD_WIDTH, CARD_HEIGHT, TFT_WHITE, TFT_BLACK);
    d.drawBitmap(136, 44, epd_bitmap_allArray[randomSplashCardIndex(Card::RANK_JACK, Card::RANK_KING)],
                 CARD_WIDTH, CARD_HEIGHT, TFT_WHITE, TFT_BLACK);

    d.setFont(&fonts::Satisfy_24);
    d.setTextDatum(textdatum_t::top_center);
    d.drawString("Blackjack", d.width() / 2, 0);
    d.setTextDatum(textdatum_t::top_left);
}

void Presenter::showSplash() const
{
    auto &d = M5.Display;
    drawSplash();
    d.drawRect(0, PROMPT_Y, d.width(), LINE_H, TFT_BLACK);
    present();
    showPrompt("Press any key", true);
    d.waitDisplay();
    waitForAnyButton();
}

void Presenter::showPrompt(const char *text, bool centered) const
{
    auto &d = M5.Display;
    beginInverseLine(PROMPT_Y);

    const int asciiWidth = d.textWidth(" ");
    d.setFont(&fonts::Font8x8C64);
    const int glyphWidth = d.textWidth(" ");
    int textWidth = 0;
    for (const char *character = text; *character; ++character)
    {
        textWidth += isButtonGlyph(static_cast<uint8_t>(*character)) ? glyphWidth : asciiWidth;
    }
    int x = textWidth < d.width()
                ? (centered ? (d.width() - textWidth) / 2 : d.width() - textWidth)
                : 0;
    for (const char *character = text; *character; ++character)
    {
        const uint8_t code = static_cast<uint8_t>(*character);
        const bool glyph = isButtonGlyph(code);
        if (glyph)
            d.setFont(&fonts::Font8x8C64);
        else
            d.setFont(&fonts::AsciiFont8x16);
        d.setCursor(x, PROMPT_Y + (glyph ? 4 : 0));
        d.write(code);
        x += glyph ? glyphWidth : asciiWidth;
    }
    d.setFont(&fonts::AsciiFont8x16);
    present();
}

void Presenter::waitForAnyButton() const
{
    M5.update();
    while (true)
    {
        M5.update();
        if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() ||
            M5.BtnC.wasPressed() || M5.BtnEXT.wasPressed())
        {
            return;
        }
        delay(20);
    }
}

int Presenter::selectMenu(const char *title, const char *const *items, int itemCount) const
{
    int selected = 0;
    bool redraw = true;
    while (true)
    {
        if (redraw)
        {
            clearUp();
            auto &d = M5.Display;
            d.setFont(&fonts::Satisfy_24);
            d.setTextDatum(textdatum_t::top_center);
            d.drawString(title, d.width() / 2, 0);
            d.setFont(&fonts::AsciiFont8x16);
            for (int i = 0; i < itemCount; ++i)
            {
                const int y = 48 + i * 28;
                if (i == selected)
                    d.fillRect(8, y - 4, d.width() - 16, 24, TFT_BLACK);
                d.setTextColor(i == selected ? TFT_WHITE : TFT_BLACK);
                d.drawString(items[i], d.width() / 2, y);
            }
            d.setTextDatum(textdatum_t::top_left);
            showPrompt("\x8C/\x8F  \x8A:Select", true);
            d.waitDisplay();
            M5.update();
            redraw = false;
        }
        M5.update();
        if (M5.BtnB.wasPressed())
            return selected;
        if (M5.BtnA.wasPressed())
        {
            selected = (selected + itemCount - 1) % itemCount;
            redraw = true;
        }
        else if (M5.BtnC.wasPressed())
        {
            selected = (selected + 1) % itemCount;
            redraw = true;
        }
        delay(20);
    }
}

Presenter::MenuAction Presenter::showMenu() const
{
    const char *const items[] = {"START GAME", "RULES", "POWER OFF"};
    const MenuAction actions[] = {MenuAction::StartGame, MenuAction::Rules, MenuAction::PowerOff};
    return actions[selectMenu("Blackjack", items, 3)];
}

int Presenter::chooseMaxScore() const
{
    const char *const items[] = {"5", "10", "25", "50"};
    const int scores[] = {5, 10, 25, 50};
    return scores[selectMenu("Play until", items, 4)];
}

void Presenter::showRules(const std::vector<std::string> &lines) const
{
    constexpr int textY = 32;
    const int linesPerPage = (PROMPT_Y - textY) / LINE_H;
    const int pageCount = lines.empty() ? 1 :
        (static_cast<int>(lines.size()) + linesPerPage - 1) / linesPerPage;
    int page = 0;
    bool redraw = true;
    while (true)
    {
        if (redraw)
        {
            clearUp();
            auto &d = M5.Display;
            char title[32];
            snprintf(title, sizeof(title), "RULES %d/%d", page + 1, pageCount);
            d.setTextDatum(textdatum_t::top_center);
            d.drawString(title, d.width() / 2, 0);
            d.setTextDatum(textdatum_t::top_left);
            for (int i = 0; i < linesPerPage; ++i)
            {
                const int index = page * linesPerPage + i;
                if (index >= static_cast<int>(lines.size()))
                    break;
                if (lines[index] != "\n")
                    drawText(0, textY + i * LINE_H, lines[index].c_str());
            }
            showPrompt("\x8C/\x8F  \x8A:Back", true);
            d.waitDisplay();
            M5.update();
            redraw = false;
        }
        M5.update();
        if (M5.BtnB.wasPressed())
            return;
        if (M5.BtnA.wasPressed() && page > 0)
        {
            --page;
            redraw = true;
        }
        else if (M5.BtnC.wasPressed() && page + 1 < pageCount)
        {
            ++page;
            redraw = true;
        }
        delay(20);
    }
}

void Presenter::showStatus(int round, const Score &score) const
{
    auto &d = M5.Display;
    beginInverseLine(ROUND_Y);
    char line[48];
    char battery[8];
    const std::string scoreText = score.toString();
    snprintf(line, sizeof(line), "Round %d  Score %s", round, scoreText.c_str());
    snprintf(battery, sizeof(battery), "%d%%", batteryPercent());

    const int batteryX = d.width() - d.textWidth(battery);
    const int availableWidth = batteryX - 8;
    if (d.textWidth(line) > availableWidth)
    {
        snprintf(line, sizeof(line), "Round %d Score: %s", round, scoreText.c_str());
    }
    if (d.textWidth(line) > availableWidth)
    {
        snprintf(line, sizeof(line), "R%d Score:%s", round, scoreText.c_str());
    }
    if (d.textWidth(line) > availableWidth)
    {
        snprintf(line, sizeof(line), "R%d:%s", round, scoreText.c_str());
    }

    drawText(0, ROUND_Y, line);
    drawText(batteryX, ROUND_Y, battery);
    d.setTextColor(TFT_BLACK);
}

// ---------------------------------------------------------
// Show player's cards (visible)
// ---------------------------------------------------------
void Presenter::showPlayerCards(const Hand &hand, int y, bool hideDealerHoleCard) const
{
    drawText(0, y + ANNOTATION_DY, hand.toString().c_str());
    const bool dealerHidden = hideDealerHoleCard && hand.size() > 1;
    drawHandTotal(y + ANNOTATION_DY + LINE_H,
                  dealerHidden ? hand.getFirstCard().getValue() : hand.getValue(),
                  dealerHidden);
    drawHand(hand, y, hideDealerHoleCard);
}

void Presenter::drawHand(const Hand &hand, int y, bool hideDealerHoleCard) const
{
    auto &d = M5.Display;
    const int cardStep = cardStepForHand(hand.size(), d.width());
    for (std::size_t index = 0; index < hand.size(); ++index)
    {
        const int x = CARD_X + static_cast<int>(index) * cardStep;
        if (x >= d.width())
        {
            break;
        }

        const Card &card = hand.getCard(index);
        const int bitmapIndex = hideDealerHoleCard && index > 0
                                    ? 0
                                    : cardBitmapIndex(card);
        d.drawBitmap(x, y, epd_bitmap_allArray[bitmapIndex],
                     CARD_WIDTH, CARD_HEIGHT, TFT_WHITE, TFT_BLACK);
    }
}

// ---------------------------------------------------------
// Reveal cards at end of round
// ---------------------------------------------------------
void Presenter::openPlayerCards(const Hand &hand,
                                const Score &score,
                                bool isWinner,
                                int y) const
{
    const char *outcome = "";

    if (score.isDraw())
    {
        outcome = "DRAW!";
    }
    else if (isWinner)
    {
        outcome = "WINS!";
    }
    else if (hand.isBust())
    {
        outcome = "BUSTS!";
    }

    showPlayerCards(hand, y);
    drawText(0, y + ANNOTATION_DY + 2 * LINE_H, outcome);
}

// ---------------------------------------------------------
// Show dealer's first card + unknown card
// ---------------------------------------------------------
void Presenter::showPlayerFirstCard(const Hand &hand, int y) const
{
    showPlayerCards(hand, y, true);
}

// ---------------------------------------------------------
// One player takes cards (dealer hidden card)
// ---------------------------------------------------------
void Presenter::onePlayerTakesCards(int round,
                                    const Hand &dealer,
                                    const Hand &player,
                                    const Score &score)
{
    clearUp();
    showStatus(round, score);
    showPlayerFirstCard(dealer, DEALER_Y);
    showPlayerCards(player, PLAYER_Y);
    present();
    delay(STEP_DELAY_MS);
}

// ---------------------------------------------------------
// Another player takes cards (both visible)
// ---------------------------------------------------------
void Presenter::anotherPlayerTakesCards(int round,
                                        const Hand &dealer,
                                        const Hand &player,
                                        const Score &score,
                                        bool revealDealerCards)
{
    clearUp();
    showStatus(round, score);
    showPlayerCards(dealer, DEALER_Y, !revealDealerCards);
    showPlayerCards(player, PLAYER_Y);
    present();
    delay(STEP_DELAY_MS);
}

// ---------------------------------------------------------
// Round finished — reveal cards
// ---------------------------------------------------------
void Presenter::roundFinished(int round,
                              const Hand &dealer,
                              const Hand &player,
                              const Score &score)
{
    clearUp();
    showStatus(round, score);
    openPlayerCards(dealer, score, score.isLeftWinner(), DEALER_Y);
    openPlayerCards(player, score, !score.isLeftWinner(), PLAYER_Y);
    showPrompt("Press any button");
    waitForAnyButton();
}

void Presenter::gameOver(const Score &score)
{
    clearUp();
    auto &d = M5.Display;
    d.setTextDatum(textdatum_t::top_center);
    d.setFont(&fonts::Satisfy_24);
    d.drawString("Game over", d.width() / 2, 0);
    d.drawString(score.leftScore() > score.rightScore() ? "Dealer won." : "You won!",
                 d.width() / 2, 64);
    char line[48];
    snprintf(line, sizeof(line), "Score %d:%d", score.leftScore(), score.rightScore());
    d.drawString(line, d.width() / 2, 90);
    d.setTextDatum(textdatum_t::top_left);
    showPrompt("Press any key", true);
    d.waitDisplay();
    waitForAnyButton();
}

void Presenter::showPowerOffMessage() const
{
    drawSplash();
    showPrompt("Power is off", true);
}

void Presenter::fatalError(const char *message)
{
    clearUp();
    beginInverseLine(ROUND_Y);
    drawText(0, ROUND_Y, "FATAL ERROR:");
    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setTextWrap(true, false);
    drawText(0, SCORE_Y, message);
    present();
}

// ---------------------------------------------------------
// Ask user for Hit or Stand (CoreInk rocker: A = up, C = down)
// ---------------------------------------------------------
bool Presenter::askPlayerHitOrStand()
{
    showPrompt("\x8C:Hit   \x8F:Stand");

    M5.update();
    while (true)
    {
        M5.update();
        if (M5.BtnA.wasPressed())
            return true;
        if (M5.BtnC.wasPressed())
            return false;
        delay(20);
    }
}
