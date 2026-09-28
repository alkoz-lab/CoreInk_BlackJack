#include <M5Unified.h>
#include "Presenter.hpp"
#include "Card.hpp"
#include <cstdio>

namespace {

constexpr int LINE_H    = 16;
constexpr int CHAR_W    = 8;
constexpr int SUIT_DY   = 4;   // centers 8px-high suit glyph in a 16px line

constexpr int ROUND_Y   = 0;
constexpr int SCORE_Y   = 16;
constexpr int DEALER_Y  = 40;
constexpr int PLAYER_Y  = 104;
constexpr int PROMPT_Y  = 184;

constexpr uint32_t STEP_DELAY_MS = 1000;

void drawText(int x, int y, const char* text) {
    auto& d = M5.Display;
    d.setFont(&fonts::AsciiFont8x16);
    d.setCursor(x, y);
    d.print(text);
}

// Draws text where bytes >= 0x80 are suit glyphs from Font8x8C64.
void drawMixed(int x, int y, const std::string& text) {
    auto& d = M5.Display;
    for (unsigned char c : text) {
        if (c >= 0x80) {
            d.setFont(&fonts::Font8x8C64);
            d.setCursor(x, y + SUIT_DY);
        } else {
            d.setFont(&fonts::AsciiFont8x16);
            d.setCursor(x, y);
        }
        d.write(c);
        x += CHAR_W;
    }
    d.setFont(&fonts::AsciiFont8x16);
}

// Draws space-separated cards, wrapping to the next line when needed.
void drawCards(int y, const std::string& cards) {
    const int width = M5.Display.width();
    int x = 0;
    size_t start = 0;

    while (start < cards.size()) {
        size_t end = cards.find(' ', start);
        if (end == std::string::npos) end = cards.size();

        std::string token = cards.substr(start, end - start);
        int tokenW = static_cast<int>(token.size()) * CHAR_W;
        if (x > 0 && x + tokenW > width) {
            x = 0;
            y += LINE_H;
        }
        drawMixed(x, y, token);
        x += tokenW + CHAR_W;
        start = end + 1;
    }
}

} // namespace

void Presenter::clearUp() const {
    auto& d = M5.Display;
    d.setEpdMode(epd_mode_t::epd_fast);
    d.fillScreen(TFT_WHITE);
    d.setTextColor(TFT_BLACK);
    d.setTextWrap(false, false);
    d.setTextDatum(textdatum_t::top_left);
    d.setFont(&fonts::AsciiFont8x16);
}

void Presenter::present() const {
    M5.Display.display();
}

void Presenter::showPrompt(const char* text) const {
    auto& d = M5.Display;
    d.fillRect(0, PROMPT_Y, d.width(), LINE_H, TFT_WHITE);
    drawText(0, PROMPT_Y, text);
    present();
}

void Presenter::waitForAnyButton() const {
    M5.update();
    while (true) {
        M5.update();
        if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() ||
            M5.BtnC.wasPressed() || M5.BtnEXT.wasPressed()) {
            return;
        }
        delay(20);
    }
}

void Presenter::showRound(int round) const {
    char line[32];
    snprintf(line, sizeof(line), "Round %d", round);
    drawText(0, ROUND_Y, line);
}

// ---------------------------------------------------------
// Show score line
// ---------------------------------------------------------
void Presenter::showScore(const Hand& dealer,
                          const Hand& player,
                          const Score& score) const {
    char line[48];
    snprintf(line, sizeof(line), "%s vs %s: %s",
             dealer.toString().c_str(),
             player.toString().c_str(),
             score.toString().c_str());
    drawText(0, SCORE_Y, line);
}

// ---------------------------------------------------------
// Show player's cards (visible)
// ---------------------------------------------------------
void Presenter::showPlayerCards(const Hand& hand, int y) const {
    char line[48];
    int value = hand.getValue();

    if (value > 0) {
        snprintf(line, sizeof(line), "%s: %d", hand.toString().c_str(), value);
    } else {
        snprintf(line, sizeof(line), "%s:", hand.toString().c_str());
    }
    drawText(0, y, line);
    drawCards(y + LINE_H, hand.getCardsString());
}

// ---------------------------------------------------------
// Reveal cards at end of round
// ---------------------------------------------------------
void Presenter::openPlayerCards(const Hand& hand,
                                const Score& score,
                                bool isWinner,
                                int y) const {
    const char* outcome = "";

    if (score.isDraw()) {
        outcome = "DRAW!";
    } else if (isWinner) {
        outcome = "WINS!";
    } else if (hand.isBust()) {
        outcome = "BUSTS!";
    }

    char line[48];
    snprintf(line, sizeof(line), "%s: %d  %s",
             hand.toString().c_str(),
             hand.getValue(),
             outcome);
    drawText(0, y, line);
    drawCards(y + LINE_H, hand.getCardsString());
}

// ---------------------------------------------------------
// Show dealer's first card + unknown card
// ---------------------------------------------------------
void Presenter::showPlayerFirstCard(const Hand& hand, int y) const {
    const Card& first = hand.getFirstCard();

    std::string cards = first.toString();
    if (hand.size() > 1) {
        cards += " " + Card('?', 0).toString();
    }

    char line[48];
    snprintf(line, sizeof(line), "%s: %d+?",
             hand.toString().c_str(),
             first.getValue());
    drawText(0, y, line);
    drawCards(y + LINE_H, cards);
}

// ---------------------------------------------------------
// One player takes cards (dealer hidden card)
// ---------------------------------------------------------
void Presenter::onePlayerTakesCards(int round,
                                    const Hand& dealer,
                                    const Hand& player,
                                    const Score& score) {
    clearUp();
    showRound(round);
    showScore(dealer, player, score);
    showPlayerFirstCard(dealer, DEALER_Y);
    showPlayerCards(player, PLAYER_Y);
    present();
    delay(STEP_DELAY_MS);
}

// ---------------------------------------------------------
// Another player takes cards (both visible)
// ---------------------------------------------------------
void Presenter::anotherPlayerTakesCards(int round,
                                        const Hand& dealer,
                                        const Hand& player,
                                        const Score& score) {
    clearUp();
    showRound(round);
    showScore(dealer, player, score);
    showPlayerCards(dealer, DEALER_Y);
    showPlayerCards(player, PLAYER_Y);
    present();
    delay(STEP_DELAY_MS);
}

// ---------------------------------------------------------
// Round finished — reveal cards
// ---------------------------------------------------------
void Presenter::roundFinished(int round,
                              const Hand& dealer,
                              const Hand& player,
                              const Score& score) {
    clearUp();
    showRound(round);
    showScore(dealer, player, score);
    openPlayerCards(dealer, score, score.isLeftWinner(), DEALER_Y);
    openPlayerCards(player, score, !score.isLeftWinner(), PLAYER_Y);
    showPrompt("Press any button");
    waitForAnyButton();
}

void Presenter::gameOver(const Score& score) {
    clearUp();
    drawText(0, ROUND_Y, "Game over!");

    char line[48];
    snprintf(line, sizeof(line), "Final score: %s", score.toString().c_str());
    drawText(0, SCORE_Y, line);
    drawText(0, DEALER_Y, score.leftScore() > score.rightScore() ? "Dealer wins." : "You win!");

    drawText(0, PLAYER_Y, "Power off.");
    drawText(0, PLAYER_Y + LINE_H, "Reset for another game.");
    present();
}

void Presenter::fatalError(const char* message) {
    clearUp();
    drawText(0, ROUND_Y, "FATAL ERROR:");
    M5.Display.setTextWrap(true, false);
    drawText(0, SCORE_Y, message);
    present();
}

// ---------------------------------------------------------
// Ask user for Hit or Stand (CoreInk rocker: A = up, C = down)
// ---------------------------------------------------------
bool Presenter::askPlayerHitOrStand() {
    showPrompt("UP:Hit   DOWN:Stand");

    M5.update();
    while (true) {
        M5.update();
        if (M5.BtnA.wasPressed()) return true;
        if (M5.BtnC.wasPressed()) return false;
        delay(20);
    }
}

