#pragma once
#include <string>
#include <vector>
#include "Hand.hpp"
#include "Score.hpp"

class Presenter
{
public:
    Presenter() = default;

    enum class MenuAction
    {
        StartGame,
        Rules,
        PowerOff
    };

    // Output
    // Shows the startup splash, then waits for a button.
    void showSplash() const;
    MenuAction showMenu() const;
    int chooseMaxScore() const;
    void showRules(const std::vector<std::string> &lines) const;
    void onePlayerTakesCards(int round, const Hand &dealer, const Hand &player, const Score &score);
    void anotherPlayerTakesCards(int round, const Hand &dealer, const Hand &player,
                                 const Score &score, bool revealDealerCards = false);
    void roundFinished(int round, const Hand &dealer, const Hand &player, const Score &score);

    void gameOver(const Score &score);
    void showPowerOffMessage() const;
    void fatalError(const char *message);

    // Input: CoreInk rocker UP = Hit, DOWN = Stand
    bool askPlayerHitOrStand();

private:
    void drawSplash() const;
    int selectMenu(const char *title, const char *const *items, int itemCount) const;
    void showPrompt(const char *text, bool centered = false) const;
    void waitForAnyButton() const;
    void showStatus(int round, const Score &score) const;
    void showPlayerCards(const Hand &hand, int y, bool hideDealerHoleCard = false) const;
    void drawHand(const Hand &hand, int y, bool hideDealerHoleCard) const;
    void openPlayerCards(const Hand &hand, const Score &score, bool isWinner, int y) const;
    void showPlayerFirstCard(const Hand &hand, int y) const;
    void clearUp() const;
    void present() const;
};
