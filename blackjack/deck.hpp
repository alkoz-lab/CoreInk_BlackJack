#pragma once
#include <vector>
#include "Card.hpp"

class Deck
{
public:
    Deck();

    void reset(); // refill to a full 52-card deck, reusing storage
    void shuffle();
    Card deal(); // will throw if deck empty

    bool empty() const;
    std::size_t size() const;

private:
    std::vector<Card> cards_;
};
