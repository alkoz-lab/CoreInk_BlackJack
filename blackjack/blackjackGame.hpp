#pragma once
#include "Deck.hpp"
#include "Hand.hpp"
#include "Score.hpp"
#include "Presenter.hpp"

class BlackjackGame {
public:
    BlackjackGame(int maxScore, Presenter& presenter);

    void playRound();
    bool isGameOver() const;

    const Score& score() const;

private:
    Deck deck_;
    Hand dealer_;
    Hand player_;
    Score score_;
    Presenter& presenter_;

    void initialDeal(int round);
    void playerTurn(int round);
    void dealerTurn(int round);
    void determineWinner();

    static std::string rules();
};
