#pragma once
#include <vector>
#include "Card.hpp"
#include "ICardSource.hpp"
#include "src/CoreInkKit/RandomSource.hpp"

class Deck : public ICardSource
{
public:
    static constexpr std::size_t CARD_COUNT = 52;

    explicit Deck(RandomSource random = hardwareRandom);

    void shuffleNewDeck() override;
    Card deal() override; // throws if the deck is empty

    void reset(); // refill to a full 52-card deck, reusing storage
    void shuffle();
    bool empty() const;
    std::size_t size() const;

private:
    std::vector<Card> cards_;
    RandomBits random_;
};
