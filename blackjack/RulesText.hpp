#pragma once
#include <cstddef>

// Rules preformatted for the CoreInk display; "\n" marks an empty line.
namespace RulesText
{
    constexpr const char *LINES[] = {
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
        "If both hands bust, the ",
        "round is a draw.",
        "Otherwise, a dealer bust ",
        "gives you a win; a player",
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
        "During play, press Down ",
        "to hit or Up to stand.",
        "On 21 you stand ",
        "automatically."};

    constexpr std::size_t LINE_COUNT = sizeof(LINES) / sizeof(LINES[0]);
}
