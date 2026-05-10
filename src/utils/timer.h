// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <chrono>
#include <thread>

namespace util {

// Time routines from the standard C++ library
struct cpp
{
    using timer_t = std::chrono::steady_clock::time_point;

    cpp() = delete;

    // Get the current time
    static timer_t StartTimer()
    {
        return std::chrono::steady_clock::now();
    }

    // Return the elapsed time in milliseconds between these call and a previous call to
    // StartTimer().
    static uint32_t EndTimer(timer_t &tmr)
    {
        auto now = std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::milliseconds>(now - tmr).count();
    }

    // Return the elapsed time in microseconds between these call and a previous call to
    // StartTimer(). Some platform might not support microsecond resolution in which
    // case this should do nothing.
    static uint32_t EndTimerUs(timer_t &tmr)
    {
        auto now = std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::microseconds>(now - tmr).count();
    }

    static void sleep(uint32_t ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    static void sleepUs(uint32_t us)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(us));
    }
};

using timer = cpp;
}
