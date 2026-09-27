#include "Card.hpp"

// -----------------------------
// Define static constants
// -----------------------------
const int Card::RANK_ACE   = 1;
const int Card::RANK_JACK  = 11;
const int Card::RANK_QUEEN = 12;
const int Card::RANK_KING  = 13;

const char Card::SUIT_CLUBS    = '\x03';
const char Card::SUIT_DIAMONDS = '\x04';
const char Card::SUIT_HEARTS   = '\x05';
const char Card::SUIT_SPADES   = '\x06';

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
    std::string s;

    switch (suit_) {
        case SUIT_CLUBS:    s = "♣"; break;
        case SUIT_DIAMONDS: s = "♦"; break;
        case SUIT_HEARTS:   s = "♥"; break;
        case SUIT_SPADES:   s = "♠"; break;
        default:            s = "?"; break;
    }

    return s;
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
