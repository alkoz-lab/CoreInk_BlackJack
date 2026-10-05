#include "HandView.hpp"
#include "CardRenderer.hpp"
#include "src/CoreInkKit/TextUtil.hpp"
#include <cstdio>

int HandView::cardStep(std::size_t cardCount, int areaWidth)
{
    constexpr int naturalStep = CardRenderer::WIDTH + 1;
    if (cardCount < 2)
        return naturalStep;

    const int maximumStep = (areaWidth - CardRenderer::WIDTH) / static_cast<int>(cardCount - 1);
    return maximumStep < naturalStep ? maximumStep : naturalStep;
}

void HandView::draw(M5GFX &d, const Hand &hand, bool hideHoleCard, bool hideNewestCard,
                    const char *resultText) const
{
    drawAnnotation(d, hand, hideHoleCard, hideNewestCard, resultText);
    drawCards(d, hand, hideHoleCard, hideNewestCard);
}

void HandView::drawAnnotation(M5GFX &d, const Hand &hand, bool hideHoleCard,
                              bool hideNewestCard, const char *resultText) const
{
    const int x = layout_.annotationX;
    const int width = layout_.annotationWidth;
    const int y = layout_.y + ANNOTATION_DY;
    TextUtil::drawCentered(d, x, width, y, hand.toString().c_str());

    const bool holeHidden = hideHoleCard && hand.size() > 1;
    int visibleValue;
    if (holeHidden)
    {
        visibleValue = hand.getFirstCard().getValue();
    }
    else if (hideNewestCard)
    {
        Hand visibleCards(hand.name());
        for (std::size_t index = 0; index + 1 < hand.size(); ++index)
            visibleCards.takeCard(hand.getCard(index));
        visibleValue = visibleCards.getValue();
    }
    else
    {
        visibleValue = hand.getValue();
    }

    char total[24];
    snprintf(total, sizeof(total), "ã=%d%s", visibleValue,
             holeHidden || hideNewestCard ? "+?" : "");
    TextUtil::drawCentered(d, x, width, y + TextUtil::LINE_HEIGHT, total);

    if (resultText && *resultText)
        TextUtil::drawCentered(d, x, width, y + 2 * TextUtil::LINE_HEIGHT, resultText);
}

void HandView::drawCards(M5GFX &d, const Hand &hand, bool hideHoleCard,
                         bool hideNewestCard) const
{
    const int step = cardStep(hand.size(), layout_.cardAreaWidth);
    const int handWidth = hand.size() == 0 ? 0 :
        CardRenderer::WIDTH + static_cast<int>(hand.size() - 1) * step;
    const int firstX = layout_.cardAreaX + (layout_.cardAreaWidth - handWidth) / 2;
    const int areaRight = layout_.cardAreaX + layout_.cardAreaWidth;

    for (std::size_t index = 0; index < hand.size(); ++index)
    {
        const int x = firstX + static_cast<int>(index) * step;
        if (x + CardRenderer::WIDTH > areaRight)
            break;

        const bool hidden = (hideHoleCard && index > 0) ||
                            (hideNewestCard && index + 1 == hand.size());
        if (hidden)
            CardRenderer::drawBack(d, x, layout_.y);
        else
            CardRenderer::drawFace(d, x, layout_.y, hand.getCard(index));
    }
}
