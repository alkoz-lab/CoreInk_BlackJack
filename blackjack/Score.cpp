#include "Score.hpp"

Score::Score(int maxScore)
    : maxScore_(maxScore)
{
}

void Score::reset()
{
    dealerScore_ = 0;
    playerScore_ = 0;
    isDealerWinner_ = false;
    isDraw_ = false;
}

std::string Score::toString() const
{
    const std::string text = std::to_string(dealerScore_) + ":" + std::to_string(playerScore_);
    if (isDraw_ || (dealerScore_ == 0 && playerScore_ == 0))
        return text;
    return isDealerWinner_ ? "+" + text : text + "+";
}

bool Score::gameOver() const
{
    return dealerScore_ >= maxScore_ || playerScore_ >= maxScore_;
}

void Score::record(Outcome outcome)
{
    switch (outcome)
    {
    case Outcome::DealerWins:
        dealerWins();
        break;
    case Outcome::PlayerWins:
        playerWins();
        break;
    case Outcome::Draw:
        draw();
        break;
    }
}

void Score::dealerWins()
{
    ++dealerScore_;
    isDealerWinner_ = true;
    isDraw_ = false;
}

void Score::playerWins()
{
    ++playerScore_;
    isDealerWinner_ = false;
    isDraw_ = false;
}

void Score::draw()
{
    isDraw_ = true;
}

int Score::dealerScore() const { return dealerScore_; }
int Score::playerScore() const { return playerScore_; }
int Score::maxScore() const { return maxScore_; }

bool Score::isDealerWinner() const { return isDealerWinner_; }
bool Score::isDraw() const { return isDraw_; }
