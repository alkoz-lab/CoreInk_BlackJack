#pragma once
#include "Hand.hpp"
#include "Outcome.hpp"
#include "Score.hpp"

enum class Side
{
    Dealer,
    Player
};

struct TableState
{
    int round;
    const Hand &dealer;
    const Hand &player;
    const Score &score;
};

// What the round controller needs to show. Implementations own pacing and animation.
class ITableView
{
public:
    virtual ~ITableView() = default;

    // Both hands dealt; the dealer's hole card stays face-down.
    virtual void showInitialDeal(const TableState &table) = 0;
    // `side` just took a card. Returns once the new card is face-up.
    // While the player hits, the dealer's hole card stays face-down.
    virtual void showHit(const TableState &table, Side side) = 0;
    // Reveals both hands and the outcome. Returns when the player is ready for the next round.
    virtual void showRoundResult(const TableState &table, Outcome outcome) = 0;
};
