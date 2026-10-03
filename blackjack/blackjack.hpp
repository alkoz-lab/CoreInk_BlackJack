#pragma once
#include <string>
#include <vector>
#include "Hand.hpp"
#include "Score.hpp"

class Blackjack
{
public:
    static void determineWinner(const Hand &dealer,
                                const Hand &player,
                                Score &score);

    static const std::vector<std::string> &rules();
};
