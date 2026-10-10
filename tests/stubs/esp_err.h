#pragma once
#include <cstdlib>

using esp_err_t = int;
constexpr esp_err_t ESP_OK = 0;

inline void testEspErrorCheck(esp_err_t error)
{
    if (error != ESP_OK)
        std::abort();
}

#define ESP_ERROR_CHECK(expression) testEspErrorCheck((expression))
