#include "RoundJudge.hpp"

Outcome RoundJudge::judge(const Hand &dealer, const Hand &player)
{
    if (dealer.isBust() && player.isBust())
        return Outcome::Draw;
    if (dealer.isBust())
        return Outcome::PlayerWins;
    if (player.isBust())
        return Outcome::DealerWins;

    const int dealerValue = dealer.getValue();
    const int playerValue = player.getValue();
    if (dealerValue == playerValue)
        return Outcome::Draw;
    return dealerValue > playerValue ? Outcome::DealerWins : Outcome::PlayerWins;
}
