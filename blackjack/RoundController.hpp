#pragma once
#include "DealerPolicy.hpp"
#include "Hand.hpp"
#include "ICardSource.hpp"
#include "IPlayerInput.hpp"
#include "ITableView.hpp"
#include "Score.hpp"

// Runs blackjack rounds against abstract cards, view and input (no hardware dependencies).
class RoundController
{
public:
    RoundController(int maxScore, ICardSource &cards, ITableView &view, IPlayerInput &input,
                    DealerPolicy dealerPolicy = DealerPolicy());

    void playRound();
    void reset();
    void reset(int maxScore);
    bool isGameOver() const;

    const Score &score() const;
    int round() const;

private:
    ICardSource &cards_;
    ITableView &view_;
    IPlayerInput &input_;
    DealerPolicy dealerPolicy_;
    Hand dealer_;
    Hand player_;
    Score score_;
    int round_ = 1;

    TableState table() const;
    void initialDeal();
    void playerTurn();
    void dealerTurn();
};
