#pragma once
#include <string>
#include "Hand.hpp"
#include "Score.hpp"

class Presenter {
public:
    Presenter(Hand& dealer, Hand& player, Score& score);

    void onePlayerTakesCards(int round);
    void anotherPlayerTakesCards(int round);
    void roundFinished(int round);

	bool askPlayerHitOrStand() const;

private:
    Hand& dealer_;
    Hand& player_;
    Score& score_;

    void showRound(int round) const;
    void showScore() const;
    void showPlayerCards(const Hand& hand) const;
    void openPlayerCards(const Hand& hand, bool isWinner) const;
    void showPlayerFirstCard(const Hand& hand) const;
    void clearUp() const;
	void showHitStandPrompt() const;
};
