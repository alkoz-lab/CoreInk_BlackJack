#include <windows.h>
#include <cstdio>
#include <exception>

#include "BlackjackGame.hpp"
#include "Presenter.hpp"

int main() {
    try {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        Presenter presenter;
        BlackjackGame game(5, presenter);

        while (!game.isGameOver()) {
            game.playRound();
        }

        printf("Game over! Final score: %s\n",
               game.score().toString().c_str());
    }
    catch (const std::exception& ex) {
        printf("FATAL ERROR: %s\n", ex.what());
    }

    return 0;
}
