#include "Score.hpp"

// -----------------------------
// Constructor
// -----------------------------
Score::Score(int max_score)
    : max_score_(max_score),
      left_score_(0),
      right_score_(0),
      is_left_winner_(false),
      is_draw_(false)
{
}

void Score::reset()
{
    left_score_ = 0;
    right_score_ = 0;
    is_left_winner_ = false;
    is_draw_ = false;
}

// -----------------------------
// toString() — match Python behavior
// -----------------------------
std::string Score::toString() const
{
    // Draw or no wins yet
    if (is_draw_ || (left_score_ == 0 && right_score_ == 0))
    {
        return std::to_string(left_score_) + ":" + std::to_string(right_score_);
    }

    // Left winner
    if (is_left_winner_)
    {
        return "+" + std::to_string(left_score_) + ":" + std::to_string(right_score_);
    }

    // Right winner
    return std::to_string(left_score_) + ":" + std::to_string(right_score_) + "+";
}

// -----------------------------
// Game over?
// -----------------------------
bool Score::gameOver() const
{
    return left_score_ >= max_score_ || right_score_ >= max_score_;
}

// -----------------------------
// Left wins
// -----------------------------
void Score::leftWins()
{
    left_score_ += 1;
    is_left_winner_ = true;
    is_draw_ = false;
}

// -----------------------------
// Right wins
// -----------------------------
void Score::rightWins()
{
    right_score_ += 1;
    is_left_winner_ = false;
    is_draw_ = false;
}

// -----------------------------
// Draw
// -----------------------------
void Score::draw()
{
    is_draw_ = true;
}

// -----------------------------
// Accessors
// -----------------------------
int Score::leftScore() const { return left_score_; }
int Score::rightScore() const { return right_score_; }
int Score::maxScore() const { return max_score_; }

bool Score::isLeftWinner() const { return is_left_winner_; }
bool Score::isDraw() const { return is_draw_; }
