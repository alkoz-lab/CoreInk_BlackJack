#pragma once
#include "Hand.hpp"
#include "Outcome.hpp"

// Pure blackjack rule: decides who wins a finished round.
namespace RoundJudge
{
    Outcome judge(const Hand &dealer, const Hand &player);
}
