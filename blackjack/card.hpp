#pragma once
#include <string>

class Card
{
public:
    // Rank constants
    static const int RANK_ACE;
    static const int RANK_JACK;
    static const int RANK_QUEEN;
    static const int RANK_KING;

    // Suit constants
    static const char SUIT_CLUBS;
    static const char SUIT_DIAMONDS;
    static const char SUIT_HEARTS;
    static const char SUIT_SPADES;

    Card(char suit, int rank);

    char suit() const;
    int rank() const;

    std::string toString() const;
    std::string getRankAsString() const;
    std::string getSuitAsString() const;

    int getValue() const;
    int getValueWhenOver21() const;

private:
    char suit_;
    int rank_;
};
