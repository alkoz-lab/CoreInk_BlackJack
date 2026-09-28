#include "Card.hpp"

// -----------------------------
// Define static constants
// -----------------------------
const int Card::RANK_ACE   = 1;
const int Card::RANK_JACK  = 11;
const int Card::RANK_QUEEN = 12;
const int Card::RANK_KING  = 13;

// Glyph codes in fonts::Font8x8C64
const char Card::SUIT_SPADES   = '\x80';
const char Card::SUIT_HEARTS   = '\x81';
const char Card::SUIT_CLUBS    = '\x82';
const char Card::SUIT_DIAMONDS = '\x83';

// -----------------------------
// Constructor
// -----------------------------
Card::Card(char suit, int rank)
    : suit_(suit), rank_(rank) {}

// -----------------------------
// Public methods
// -----------------------------
char Card::suit() const {
    return suit_;
}

int Card::rank() const {
    return rank_;
}

std::string Card::toString() const {
    return getRankAsString() + getSuitAsString();
}

std::string Card::getRankAsString() const {
    std::string r;

    if (rank_ == 0)               r = "?";
    else if (rank_ == RANK_JACK)  r = "J";
    else if (rank_ == RANK_QUEEN) r = "Q";
    else if (rank_ == RANK_KING)  r = "K";
    else if (rank_ == RANK_ACE)   r = "A";
    else                          r = std::to_string(rank_);

    return r;
}

std::string Card::getSuitAsString() const {
    switch (suit_) {
        case SUIT_CLUBS:
        case SUIT_DIAMONDS:
        case SUIT_HEARTS:
        case SUIT_SPADES:
            return std::string(1, suit_);
        default:
            return "?";
    }
}

int Card::getValue() const {
    if (rank_ == RANK_ACE) return 11;
    if (rank_ >= 10)       return 10;
    return rank_;
}

int Card::getValueWhenOver21() const {
    if (rank_ == RANK_ACE) return 1;
    if (rank_ >= 10)       return 10;
    return rank_;
}
