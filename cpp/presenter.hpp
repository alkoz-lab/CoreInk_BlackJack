#pragma once
#include "Hand.hpp"
#include "Score.hpp"

class Presenter {
public:
    Presenter() = default;

    // Output
    void onePlayerTakesCards(int round, const Hand& dealer, const Hand& player, const Score& score);
    void anotherPlayerTakesCards(int round, const Hand& dealer, const Hand& player, const Score& score);
    void roundFinished(int round, const Hand& dealer, const Hand& player, const Score& score);

    // Input (desktop or Arduino)
    bool askPlayerHitOrStand();

private:
    void showHitStandPrompt() const;
    void showRound(int round) const;
    void showScore(const Hand& dealer, const Hand& player, const Score& score) const;
    void showPlayerCards(const Hand& hand) const;
    void openPlayerCards(const Hand& hand, const Score& score, bool isWinner) const;
    void showPlayerFirstCard(const Hand& hand) const;
    void clearUp() const;
};
