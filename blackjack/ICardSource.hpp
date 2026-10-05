#pragma once
#include "Card.hpp"

// Where the round controller gets its cards from.
class ICardSource
{
public:
    virtual ~ICardSource() = default;
    virtual void shuffleNewDeck() = 0;
    virtual Card deal() = 0;
};
