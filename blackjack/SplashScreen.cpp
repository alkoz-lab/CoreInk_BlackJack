#include "SplashScreen.hpp"
#include "CardRenderer.hpp"
#include "src/CoreInkKit/EinkFrame.hpp"
#include "src/CoreInkKit/RandomSource.hpp"
#include "src/CoreInkKit/TextUtil.hpp"
#include <random>

namespace
{
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
}

void SplashScreen::show()
{
    auto &d = display_;
    drawArt();
    prompt_.drawOutline(d);
    EinkFrame::present(d);
    prompt_.draw(d, "Press any key");
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
