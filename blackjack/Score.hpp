#pragma once
#include <string>
#include "Outcome.hpp"

class Score
{
public:
    explicit Score(int maxScore);

    // Status-line text such as "+1:0" (dealer:player, '+' marks the last winner).
    std::string toString() const;
    void reset();

    bool gameOver() const;

    void record(Outcome outcome);
    void dealerWins();
    void playerWins();
    void draw();

    int dealerScore() const;
    int playerScore() const;
    int maxScore() const;

    bool isDealerWinner() const;
    bool isDraw() const;

private:
    int maxScore_;
    int dealerScore_ = 0;
    int playerScore_ = 0;
    bool isDealerWinner_ = false;
    bool isDraw_ = false;
};
