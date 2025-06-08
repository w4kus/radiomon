#pragma once

#include <chrono>

namespace util {
    
class timer
{
public:

    typedef std::chrono::steady_clock::time_point timer_t;

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
