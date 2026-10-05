#pragma once
#include <cstdio>
#include <cstdlib>

inline void require(bool condition, const char *message)
{
    if (!condition)
    {
        std::fprintf(stderr, "Failed: %s\n", message);
        std::exit(EXIT_FAILURE);
    }
}