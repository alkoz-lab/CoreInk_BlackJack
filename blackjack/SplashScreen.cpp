#include "SplashScreen.hpp"
#include "CardRenderer.hpp"
#include "src/CoreInkKit/EinkFrame.hpp"
#include "src/CoreInkKit/RandomSource.hpp"
#include "src/CoreInkKit/TextUtil.hpp"
#include <random>

namespace
{
    constexpr char APP_VERSION[] = "1.0.0";
    constexpr int VERSION_FONT_HEIGHT = 8;

    Card randomCard(int firstRank, int lastRank)
    {
        RandomBits generator{hardwareRandom};
        const char suits[] = {Card::SUIT_CLUBS, Card::SUIT_DIAMONDS,
                              Card::SUIT_HEARTS, Card::SUIT_SPADES};
        std::uniform_int_distribution<int> rank(firstRank, lastRank);
        std::uniform_int_distribution<int> suit(0, 3);
        return Card(suits[suit(generator)], rank(generator));
    }
}

void SplashScreen::drawArt()
{
    auto &d = display_;
    EinkFrame::begin(d);
    CardRenderer::drawBack(d, 8, 34);
    CardRenderer::drawFace(d, 39, 49, randomCard(10, 10));
    CardRenderer::drawFace(d, 86, 75, randomCard(Card::RANK_ACE, Card::RANK_ACE));
    CardRenderer::drawFace(d, 136, 44, randomCard(Card::RANK_JACK, Card::RANK_KING));
    TextUtil::drawTitle(d, "Blackjack");
    d.setFont(&fonts::Font8x8C64);
    d.setTextColor(TFT_BLACK);
    d.setCursor(d.width() - d.textWidth(APP_VERSION),
                prompt_.y() - VERSION_FONT_HEIGHT);
    d.print(APP_VERSION);
    d.setFont(&fonts::AsciiFont8x16);
}

void SplashScreen::show()
{
    auto &d = display_;
    drawArt();
    prompt_.drawOutline(d);
    EinkFrame::present(d);
    prompt_.draw(d, "Press any button");
    EinkFrame::present(d);
    d.waitDisplay();
    buttons_.waitForAny();
}

void SplashScreen::showPowerOff()
{
    drawArt();
    prompt_.draw(display_, "Power is off");
    EinkFrame::present(display_);
}
