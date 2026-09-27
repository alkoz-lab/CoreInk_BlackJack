#include "Presenter.hpp"
#include "Card.hpp"
#include <cstdio>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <conio.h>

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
Presenter::Presenter(Hand& dealer, Hand& player, Score& score)
    : dealer_(dealer), player_(player), score_(score)
{}

// ---------------------------------------------------------
// One player takes cards (dealer hidden card)
// ---------------------------------------------------------
void Presenter::onePlayerTakesCards(int round) {
    clearUp();
    showRound(round);
    showScore();
    showPlayerFirstCard(dealer_);
    showPlayerCards(player_);
    printf("\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// ---------------------------------------------------------
// Another player takes cards (both visible)
// ---------------------------------------------------------
void Presenter::anotherPlayerTakesCards(int round) {
    clearUp();
    showRound(round);
    showScore();
    showPlayerCards(dealer_);
    showPlayerCards(player_);
    printf("\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// ---------------------------------------------------------
// Round finished — reveal cards
// ---------------------------------------------------------
void Presenter::roundFinished(int round) {
    clearUp();
    showRound(round);
    showScore();
    openPlayerCards(dealer_, score_.isLeftWinner());
    openPlayerCards(player_, !score_.isLeftWinner());
    printf("\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// ---------------------------------------------------------
// Show round number
// ---------------------------------------------------------
void Presenter::showRound(int round) const {
    printf("Round %d ", round);
}

// ---------------------------------------------------------
// Show score line
// ---------------------------------------------------------
void Presenter::showScore() const {
    printf("%s vs %s score: %s\n\n",
           dealer_.toString().c_str(),
           player_.toString().c_str(),
           score_.toString().c_str());
}

// ---------------------------------------------------------
// Show player's cards (visible)
// ---------------------------------------------------------
void Presenter::showPlayerCards(const Hand& hand) const {
    int value = hand.getValue();

    if (value > 0) {
        printf("%-15s%s : %d\n",
               hand.toString().c_str(),
               hand.getCardsString().c_str(),
               value);
    } else {
        printf("%s:\n", hand.toString().c_str());
    }
}

// ---------------------------------------------------------
// Reveal cards at end of round
// ---------------------------------------------------------
void Presenter::openPlayerCards(const Hand& hand, bool isWinner) const {
    const char* outcome = "";

    if (score_.isDraw()) {
        outcome = "DRAW!";
    } else if (isWinner) {
        outcome = "WINS!";
    } else if (hand.isBust()) {
        outcome = "BUSTS!";
    }

    printf("%-15s%s : %d\t%s\n",
           hand.toString().c_str(),
           hand.getCardsString().c_str(),
           hand.getValue(),
           outcome);
}

// ---------------------------------------------------------
// Show dealer's first card + unknown card
// ---------------------------------------------------------
void Presenter::showPlayerFirstCard(const Hand& hand) const {
    char unknown[64] = " ";
    if (hand.size() > 1) {
        Card unknownCard('?', 0);
        snprintf(unknown + 1, sizeof(unknown) - 1, "%s", unknownCard.toString().c_str());
    }

    const Card& first = hand.getFirstCard();

    printf("%-15s%s%s : %d+?\n",
           hand.toString().c_str(),
           first.toString().c_str(),
           unknown,
           first.getValue());
}

// ---------------------------------------------------------
// Clear terminal
// ---------------------------------------------------------
void Presenter::clearUp() const {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

// ---------------------------------------------------------
// Hit/Stand prompt
// ---------------------------------------------------------
void Presenter::showHitStandPrompt() const {
    printf("\nChoose your action:\n");
    printf("  ↑  Hit (take another card)\n");
    printf("  ↓  Stand (end your turn)\n");
    printf("Press UP or DOWN on your device.\n\n");
}

// ---------------------------------------------------------
// Ask for Hit/Stand player action
// ---------------------------------------------------------
bool Presenter::askPlayerHitOrStand() const {
	
	showHitStandPrompt();
	
    while (true) {
        int ch = _getch();

        // Arrow keys: first 0 or 224, then actual code
        if (ch == 0 || ch == 224) {
            int arrow = _getch();
            if (arrow == 72) return true;   // UP
            if (arrow == 80) return false;  // DOWN
        }
    }
}
