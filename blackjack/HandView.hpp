#pragma once
#include <M5Unified.h>
#include <cstddef>
#include "Hand.hpp"

// Where one hand goes on screen: a row of cards beside a column of annotation text.
struct HandLayout
{
    int y;
    int annotationX;
    int annotationWidth;
    int cardAreaX;
    int cardAreaWidth;
};

// Draws one hand: cards centered (compacted when needed) in the card area, and the
// name, visible total and optional result centered in the annotation column. Draw-only.
class HandView
{
public:
    static constexpr int ANNOTATION_DY = 16;

    explicit HandView(const HandLayout &layout) : layout_(layout) {}

    // hideHoleCard: every card after the first stays face-down (dealer before the reveal).
    // hideNewestCard: the newest card stays face-down and is left out of the total.
    void draw(M5GFX &d, const Hand &hand, bool hideHoleCard, bool hideNewestCard,
              const char *resultText = nullptr) const;

    // Distance between card left edges: natural spacing, or overlapped to fit the area.
    static int cardStep(std::size_t cardCount, int areaWidth);

private:
    HandLayout layout_;

    void drawAnnotation(M5GFX &d, const Hand &hand, bool hideHoleCard, bool hideNewestCard,
                        const char *resultText) const;
    void drawCards(M5GFX &d, const Hand &hand, bool hideHoleCard, bool hideNewestCard) const;
};
