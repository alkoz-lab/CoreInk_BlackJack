#pragma once
#include "Hand.hpp"

// Dealer drawing strategy, kept separate so house rules can change without touching the round flow.
class DealerPolicy
{
public:
    static constexpr int DEFAULT_STAND_VALUE = 17;

    explicit constexpr DealerPolicy(int standValue = DEFAULT_STAND_VALUE)
        : standValue_(standValue)
    {
    }

    bool shouldHit(const Hand &dealer) const { return dealer.getValue() < standValue_; }
    int standValue() const { return standValue_; }

private:
    int standValue_;
};
