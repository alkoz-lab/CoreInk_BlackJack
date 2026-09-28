#include "Hand.hpp"

// -----------------------------
// Static constant
// -----------------------------
const int Hand::BLACKJACK_VALUE = 21;

// -----------------------------
// Constructor
// -----------------------------
Hand::Hand(const std::string& name)
    : name_(name)
{
    cards_.clear();
}

// -----------------------------
// Number of cards in hand
// -----------------------------
std::size_t Hand::size() const {
    return cards_.size();
}

// -----------------------------
// toString() — same as Python __str__
// -----------------------------
std::string Hand::toString() const {
    return name_;
}

// -----------------------------
// Return "Q♥ 10♣ A♠"
// -----------------------------
std::string Hand::getCardsString() const {
    std::string out;

    for (std::size_t i = 0; i < cards_.size(); ++i) {
        out += cards_[i].toString();
        if (i + 1 < cards_.size()) {
            out += " ";
        }
    }

    return out;
}

// -----------------------------
// First card (dealer hole card logic)
// -----------------------------
const Card& Hand::getFirstCard() const {
    return cards_.front();
}

// -----------------------------
// Blackjack value calculation
// Matches Python logic exactly
// -----------------------------
int Hand::getValue() const {
    int value_no_aces = 0;
    std::vector<Card> aces;

    // Separate aces from non-aces
    for (const Card& card : cards_) {
        if (card.rank() == Card::RANK_ACE) {
            aces.push_back(card);
        } else {
            value_no_aces += card.getValue();
        }
    }

    // Count aces
    int aces_count = static_cast<int>(aces.size());
    int value_only_aces = 0;

    if (aces_count > 0) {
        // Use a dummy ace to get values
        Card ace(Card::SUIT_HEARTS, Card::RANK_ACE);
        int ace_max_value = ace.getValue();               // 11
        int ace_min_value = ace.getValueWhenOver21();     // 1

        // Check if one ace can be counted as 11
        if (value_no_aces + ace_min_value * (aces_count - 1) + ace_max_value
            <= BLACKJACK_VALUE)
        {
            value_only_aces = ace_min_value * (aces_count - 1) + ace_max_value;
        }
        else {
            value_only_aces = ace_min_value * aces_count;
        }
    }

    return value_no_aces + value_only_aces;
}

// -----------------------------
// Bust check
// -----------------------------
bool Hand::isBust() const {
    return getValue() > BLACKJACK_VALUE;
}

// -----------------------------
// Add card to hand
// -----------------------------
void Hand::takeCard(const Card& card) {
    cards_.push_back(card);
}

// -----------------------------
// Accessor
// -----------------------------
const std::string& Hand::name() const {
    return name_;
}
