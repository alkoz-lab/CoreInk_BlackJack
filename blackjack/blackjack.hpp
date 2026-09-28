#pragma once
#include <string>
#include "Hand.hpp"
#include "Score.hpp"

class Blackjack {
public:
    static void determineWinner(const Hand& dealer,
                                const Hand& player,
                                Score& score);

    static std::string rules();
};
