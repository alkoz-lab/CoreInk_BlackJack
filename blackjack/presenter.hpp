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

    void gameOver(const Score& score);
    void fatalError(const char* message);

    // Input: CoreInk rocker UP = Hit, DOWN = Stand
    bool askPlayerHitOrStand();

private:
    void showPrompt(const char* text) const;
    void waitForAnyButton() const;
    void showRound(int round) const;
    void showScore(const Hand& dealer, const Hand& player, const Score& score) const;
    void showPlayerCards(const Hand& hand, int y) const;
    void openPlayerCards(const Hand& hand, const Score& score, bool isWinner, int y) const;
    void showPlayerFirstCard(const Hand& hand, int y) const;
    void clearUp() const;
    void present() const;
};
