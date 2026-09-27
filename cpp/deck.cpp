#include "Deck.hpp"
#include <algorithm>
#include <random>

// -----------------------------
// Constructor: build full deck
// -----------------------------
Deck::Deck() {
    cards_.reserve(52);

    const char suits[] = {
        Card::SUIT_CLUBS,
        Card::SUIT_DIAMONDS,
        Card::SUIT_HEARTS,
        Card::SUIT_SPADES
    };

    for (char suit : suits) {
        for (int rank = 1; rank <= 13; ++rank) {
            cards_.emplace_back(suit, rank);
        }
    }
}

// -----------------------------
// Shuffle using C++ RNG
// -----------------------------
void Deck::shuffle() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(cards_.begin(), cards_.end(), gen);
}

// -----------------------------
// Deal: pop from end (Python pop())
// -----------------------------
Card Deck::deal() {
	if (empty()) {
        throw std::runtime_error("Deck is out of cards — cannot deal.");
    }
	
    Card c = cards_.back();
    cards_.pop_back();
    return c;
}

// -----------------------------
// Helpers
// -----------------------------
bool Deck::empty() const {
    return cards_.empty();
}

std::size_t Deck::size() const {
    return cards_.size();
}
