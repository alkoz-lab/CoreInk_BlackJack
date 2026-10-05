#include "Deck.hpp"
#include <algorithm>
#include <stdexcept>

Deck::Deck(RandomSource random)
    : random_{random}
{
    cards_.reserve(CARD_COUNT);
    reset();
}

void Deck::shuffleNewDeck()
{
    reset();
    shuffle();
}

void Deck::reset()
{
    cards_.clear();

    const char suits[] = {
        Card::SUIT_CLUBS,
        Card::SUIT_DIAMONDS,
        Card::SUIT_HEARTS,
        Card::SUIT_SPADES};

    for (char suit : suits)
    {
        for (int rank = Card::RANK_ACE; rank <= Card::RANK_KING; ++rank)
        {
            cards_.emplace_back(suit, rank);
        }
    }
}

void Deck::shuffle()
{
    std::shuffle(cards_.begin(), cards_.end(), random_);
}

Card Deck::deal()
{
    if (empty())
    {
        throw std::runtime_error("Deck is out of cards — cannot deal.");
    }

    Card card = cards_.back();
    cards_.pop_back();
    return card;
}

bool Deck::empty() const
{
    return cards_.empty();
}

std::size_t Deck::size() const
{
    return cards_.size();
}
