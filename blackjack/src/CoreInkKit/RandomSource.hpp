#pragma once
#include <cstdint>
#include <limits>
#include <esp_random.h>

// A function returning 32 random bits; injectable so tests can be deterministic.
using RandomSource = uint32_t (*)();

// ESP32 hardware RNG. Calling it per draw (instead of seeding a PRNG once)
// keeps every one of the 52! deck orders reachable.
inline uint32_t hardwareRandom()
{
    return esp_random();
}

// Adapts a RandomSource to the C++ UniformRandomBitGenerator concept.
struct RandomBits
{
    using result_type = uint32_t;
    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
    result_type operator()() const { return source(); }

    RandomSource source;
};
