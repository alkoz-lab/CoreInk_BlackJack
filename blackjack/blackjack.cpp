#include "Blackjack.hpp"

// ---------------------------------------------------------
// determineWinner — faithful translation of Python logic
// ---------------------------------------------------------
void Blackjack::determineWinner(const Hand &dealer,
                                const Hand &player,
                                Score &score)
{
    int dealer_value = dealer.getValue();
    int player_value = player.getValue();

    if (dealer.isBust())
    {
        score.rightWins(); // player wins
    }
    else if (player.isBust())
    {
        score.leftWins(); // dealer wins
    }
    else if (dealer_value == player_value)
    {
        score.draw();
    }
    else if (dealer_value > player_value)
    {
        score.leftWins(); // dealer wins
    }
    else
    {
        score.rightWins(); // player wins
    }
}

// ---------------------------------------------------------
// rules() — returns the same text as Python version
// ---------------------------------------------------------
std::string Blackjack::rules()
{
    return "Blackjack is a card game where players try to get a hand value\n"
           "of 21 or as close to 21 as possible without going over.\n"
           "Players are dealt two cards and can choose to 'hit' for\n"
           "additional cards or 'stand' to keep their current hand.\n"
           "The dealer also receives two cards, one face-up and one face-down.\n"
           "The dealer must hit until their hand value is at least 17.\n"
           "If a player's hand value is higher than the dealer's without\n"
           "going over 21, the player wins.";
}
