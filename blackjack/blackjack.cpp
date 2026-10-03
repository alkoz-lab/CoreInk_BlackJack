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
// Rules preformatted for the CoreInk display.
// ---------------------------------------------------------
const std::vector<std::string> &Blackjack::rules()
{
    static const std::vector<std::string> lines = {
        "Blackjack is a card game ",
        "where players try to get ",
        "a hand value of 21 or as ",
        "close to 21 as possible ",
        "without going over.",
        "\n",
        "Players are dealt two ",
        "cards and can choose to ",
        "'hit' for additional ",
        "cards or 'stand' to keep ",
        "their current hand.",
        "\n",
        "The dealer also receives ",
        "two cards, one face-up ",
        "and one face-down.",
        "The dealer must hit until",
        "their hand value is at ",
        "least 17.",
        "If a player's hand value ",
        "is higher than the ",
        "dealer's without going ",
        "over 21, the player wins.",
        "\n",
        "Number cards count as ",
        "their number.",
        "Jacks, queens and kings ",
        "count as 10.",
        "Aces count as 11 or 1 to ",
        "avoid going over 21.",
        "\n",
        "Going over 21 is a bust.",
        "A dealer bust gives you a",
        "win; otherwise a player ",
        "bust gives the dealer a ",
        "win.",
        "Equal totals are a draw.",
        "Each win adds one point;",
        "a draw adds none.",
        "\n",
        "The first to the ",
        "selected score wins the ",
        "game.",
        "\n",
        "During play, press Up to",
        "hit or Down to stand."};
    return lines;
}
