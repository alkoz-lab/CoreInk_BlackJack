#pragma once
#include <string>
#include <vector>
#include "Card.hpp"

class Hand {
public:
    static const int BLACKJACK_VALUE;

    Hand(const std::string& name);
	
	std::size_t size() const;

    std::string toString() const;
    std::string getCardsString() const;

    const Card& getFirstCard() const;

    int getValue() const;
    bool isBust() const;

    void takeCard(const Card& card);

    const std::string& name() const;

private:
    std::string name_;
    std::vector<Card> cards_;
};
