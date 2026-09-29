#pragma once
#include <string>

class Score
{
public:
    Score(int max_score);

    std::string toString() const;

    bool gameOver() const;

    void leftWins();
    void rightWins();
    void draw();

    int leftScore() const;
    int rightScore() const;
    int maxScore() const;

    bool isLeftWinner() const;
    bool isDraw() const;

private:
    int max_score_;
    int left_score_;
    int right_score_;
    bool is_left_winner_;
    bool is_draw_;
};
