// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <chrono>

namespace util {
    
class timer
{
public:

    using timer_t = std::chrono::steady_clock::time_point;

    timer() = delete;

    static timer_t StartTimer()
    {
        return std::chrono::steady_clock::now();
    }

    static uint32_t EndTimer(timer_t &tmr)
    {
        auto now = std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::milliseconds>(now - tmr).count();
    }

    static uint32_t EndTimerUs(timer_t &tmr)
    {
        auto now = std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::microseconds>(now - tmr).count();
    }
};
}
