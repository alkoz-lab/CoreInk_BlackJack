#include "../blackjack/Deck.hpp"
#include "check.hpp"
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace
{
    uint32_t state = 12345;
    uint32_t testRandom()
    {
        state = state * 1664525u + 1013904223u;
        return state;
    }

    std::vector<std::pair<char, int>> dealAll(Deck &deck)
    {
        std::vector<std::pair<char, int>> cards;
        while (!deck.empty())
        {
            const Card card = deck.deal();
            cards.emplace_back(card.suit(), card.rank());
        }
        return cards;
    }
}

int main()
{
    Deck deck(testRandom);
    deck.shuffleNewDeck();
    const auto first = dealAll(deck);
    require(first.size() == Deck::CARD_COUNT, "a new deck has 52 cards");
    require(std::set<std::pair<char, int>>(first.begin(), first.end()).size() == Deck::CARD_COUNT,
            "every card is unique");

    deck.shuffleNewDeck();
    require(dealAll(deck) != first, "consecutive shuffles differ");

    bool threw = false;
    try
    {
        deck.deal();
    }
    catch (const std::runtime_error &)
    {
        threw = true;
    }
    require(threw, "dealing from an empty deck throws");

    std::puts("All deck tests passed.");
    return EXIT_SUCCESS;
}