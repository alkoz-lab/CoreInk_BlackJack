#include <windows.h>        // for UTF‑8 console on Windows
#include <cstdio>
#include <thread>
#include <chrono>

#include "Deck.hpp"
#include "Hand.hpp"
#include "Score.hpp"
#include "Blackjack.hpp"
#include "Presenter.hpp"

int main() {
    // Enable UTF‑8 output on Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

	try{
		Score score(10);

		Hand dealer("Dealer");
		Hand player("You");

		Presenter presenter(dealer, player, score);

		int round = 1;

		while (!score.gameOver()) {

			// Reset hands
			dealer = Hand("Dealer");
			player = Hand("You");

			Deck deck;
			deck.shuffle();

			// Initial deal
			dealer.takeCard(deck.deal());
			dealer.takeCard(deck.deal());

			player.takeCard(deck.deal());
			player.takeCard(deck.deal());

			// Show initial state (dealer hidden card)
			presenter.onePlayerTakesCards(round);

			// Player turn
			bool player_stands = false;

			while (!player_stands && !player.isBust()) {

				bool hit = presenter.askPlayerHitOrStand();

				if (hit) {
					player.takeCard(deck.deal());
					presenter.anotherPlayerTakesCards(round);
				} else {
					player_stands = true;
				}
			}

			// Dealer turn (simple Blackjack rule: hit until >= 17)
			while (dealer.getValue() < 17) {
				dealer.takeCard(deck.deal());
				presenter.anotherPlayerTakesCards(round);
			}

			// Determine winner
			Blackjack::determineWinner(dealer, player, score);

			// Show final round result
			presenter.roundFinished(round);

			round++;
		}

		printf("Game over! Final score: %s\n", score.toString().c_str());
		return 0;
	}
    catch (const std::exception& ex) {
        printf("FATAL ERROR: %s\n", ex.what());
        return 1;
    }
}
