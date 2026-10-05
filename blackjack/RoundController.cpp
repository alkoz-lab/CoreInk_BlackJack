#include "RoundController.hpp"
#include "RoundJudge.hpp"

namespace
{
    const char *const DEALER_NAME = "DEALER";
    const char *const PLAYER_NAME = "PLAYER";
}

RoundController::RoundController(int maxScore, ICardSource &cards, ITableView &view,
                                 IPlayerInput &input, DealerPolicy dealerPolicy)
    : cards_(cards),
      view_(view),
      input_(input),
      dealerPolicy_(dealerPolicy),
      dealer_(DEALER_NAME),
      player_(PLAYER_NAME),
      score_(maxScore)
{
}

bool RoundController::isGameOver() const
{
    return score_.gameOver();
}

const Score &RoundController::score() const
{
    return score_;
}

int RoundController::round() const
{
    return round_;
}

void RoundController::reset()
{
    dealer_.clear();
    player_.clear();
    score_.reset();
    round_ = 1;
}

void RoundController::reset(int maxScore)
{
    score_ = Score(maxScore);
    reset();
}

TableState RoundController::table() const
{
    return {round_, dealer_, player_, score_};
}

void RoundController::playRound()
{
    cards_.shuffleNewDeck();

    initialDeal();
    playerTurn();
    dealerTurn();

    const Outcome outcome = RoundJudge::judge(dealer_, player_);
    score_.record(outcome);
    view_.showRoundResult(table(), outcome);

    ++round_;
}

void RoundController::initialDeal()
{
    dealer_.clear();
    player_.clear();

    dealer_.takeCard(cards_.deal());
    dealer_.takeCard(cards_.deal());

    player_.takeCard(cards_.deal());
    player_.takeCard(cards_.deal());

    view_.showInitialDeal(table());
}

void RoundController::playerTurn()
{
    // Bust ends the turn, and 21 cannot be improved, so the player stands automatically.
    while (player_.getValue() < Hand::BLACKJACK_VALUE &&
           input_.askHitOrStand() == IPlayerInput::Decision::Hit)
    {
        player_.takeCard(cards_.deal());
        view_.showHit(table(), Side::Player);
    }
}

void RoundController::dealerTurn()
{
    while (dealerPolicy_.shouldHit(dealer_))
    {
        dealer_.takeCard(cards_.deal());
        view_.showHit(table(), Side::Dealer);
    }
}
