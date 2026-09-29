#include "BlackjackGame.hpp"
#include "Blackjack.hpp"
#include <stdexcept>

const char *DEALER_NAME = "DEALER";
const char *PLAYER_NAME = "PLAYER";

BlackjackGame::BlackjackGame(int maxScore, Presenter &presenter)
    : deck_(),
      dealer_(DEALER_NAME),
      player_(PLAYER_NAME),
      score_(maxScore),
      presenter_(presenter)
{
}

bool BlackjackGame::isGameOver() const
{
    return score_.gameOver();
}

const Score &BlackjackGame::score() const
{
    return score_;
}

void BlackjackGame::playRound()
{
    deck_.reset();
    deck_.shuffle();

    initialDeal(round_);
    playerTurn(round_);
    dealerTurn(round_);
    determineWinner();

    presenter_.roundFinished(round_, dealer_, player_, score_);

    ++round_;
}

void BlackjackGame::initialDeal(int round)
{
    dealer_.clear();
    player_.clear();

    dealer_.takeCard(deck_.deal());
    dealer_.takeCard(deck_.deal());

    player_.takeCard(deck_.deal());
    player_.takeCard(deck_.deal());

    presenter_.onePlayerTakesCards(round, dealer_, player_, score_);
}

void BlackjackGame::playerTurn(int round)
{
    bool player_stands = false;

    while (!player_stands && !player_.isBust())
    {
        bool hit = presenter_.askPlayerHitOrStand();

        if (hit)
        {
            player_.takeCard(deck_.deal());
            presenter_.anotherPlayerTakesCards(round, dealer_, player_, score_);
        }
        else
        {
            player_stands = true;
        }
    }
}

void BlackjackGame::dealerTurn(int round)
{
    while (dealer_.getValue() < 17)
    {
        dealer_.takeCard(deck_.deal());
        presenter_.anotherPlayerTakesCards(round, dealer_, player_, score_);
    }
}

void BlackjackGame::determineWinner()
{
    Blackjack::determineWinner(dealer_, player_, score_);
}
