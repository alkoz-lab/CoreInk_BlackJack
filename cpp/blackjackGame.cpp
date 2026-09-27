#include "BlackjackGame.hpp"
#include "Blackjack.hpp"
#include <stdexcept>

BlackjackGame::BlackjackGame(int maxScore, Presenter& presenter)
    : deck_(),
      dealer_("Dealer"),
      player_("You"),
      score_(maxScore),
      presenter_(presenter)
{
}

bool BlackjackGame::isGameOver() const {
    return score_.gameOver();
}

const Score& BlackjackGame::score() const {
    return score_;
}

void BlackjackGame::playRound() {
    static int round = 1;

    deck_ = Deck();
    deck_.shuffle();

    initialDeal(round);
    playerTurn(round);
    dealerTurn(round);
    determineWinner();

    presenter_.roundFinished(round, dealer_, player_, score_);

    ++round;
}

void BlackjackGame::initialDeal(int round) {
    dealer_ = Hand("Dealer");
    player_ = Hand("You");

    dealer_.takeCard(deck_.deal());
    dealer_.takeCard(deck_.deal());

    player_.takeCard(deck_.deal());
    player_.takeCard(deck_.deal());

    presenter_.onePlayerTakesCards(round, dealer_, player_, score_);
}

void BlackjackGame::playerTurn(int round) {
    bool player_stands = false;

    while (!player_stands && !player_.isBust()) {
        bool hit = presenter_.askPlayerHitOrStand();

        if (hit) {
            player_.takeCard(deck_.deal());
            presenter_.anotherPlayerTakesCards(round, dealer_, player_, score_);
        } else {
            player_stands = true;
        }
    }
}

void BlackjackGame::dealerTurn(int round) {
    while (dealer_.getValue() < 17) {
        dealer_.takeCard(deck_.deal());
        presenter_.anotherPlayerTakesCards(round, dealer_, player_, score_);
    }
}

void BlackjackGame::determineWinner() {
    Blackjack::determineWinner(dealer_, player_, score_);
}
