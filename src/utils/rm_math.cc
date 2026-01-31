// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "rm-math.h"

using namespace util;

typedef struct
{
    float product;
    float inverse;
}round_tab_t;

const round_tab_t round_tab[] =
{
    [0] = { .product = 1.0e0f, .inverse = 1.0e-0f },
    [1] = { .product = 1.0e1f, .inverse = 1.0e-1f },
    [2] = { .product = 1.0e2f, .inverse = 1.0e-2f },
    [3] = { .product = 1.0e3f, .inverse = 1.0e-3f },
    [4] = { .product = 1.0e4f, .inverse = 1.0e-4f },
    [5] = { .product = 1.0e5f, .inverse = 1.0e-5f },
    [6] = { .product = 1.0e6f, .inverse = 1.0e-6f },
    [7] = { .product = 1.0e7f, .inverse = 1.0e-7f }
};

constexpr size_t ROUND_TAB_SIZE = (sizeof(round_tab) / sizeof(round_tab[0]));

float volk::round(float value, uint8_t digits)
{
    float ret = 0.0f;

    if (digits < ROUND_TAB_SIZE)
    {
        float i;
        ret = std::round(std::modf(value, &i) * round_tab[digits].product);
        ret = ret * round_tab[digits].inverse + i;
    }

    return ret;
}
