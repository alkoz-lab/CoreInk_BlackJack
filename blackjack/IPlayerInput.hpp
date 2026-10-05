#pragma once

class IPlayerInput
{
public:
    enum class Decision
    {
        Hit,
        Stand
    };

    virtual ~IPlayerInput() = default;
    virtual Decision askHitOrStand() = 0;
};
