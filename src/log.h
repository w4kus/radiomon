#pragma once

#include <memory>
#include <chrono>

namespace dmr {

class log
{
public:

    typedef enum
    {
        MAIN = 1,
        CORR = 2,

        ALL = MAIN | CORR
    }modname_t;

    typedef std::chrono::steady_clock::time_point timer_t;

    static auto &getInst()
    {
        static std::shared_ptr<log> p{ new log };
        return p;
    }

    log(const log&) = delete;
    log& operator=(const log&) = delete;

private:

    log() : m_ActiveModes(0)
    {
        m_Start = std::chrono::steady_clock::now();
    }

    uint8_t m_ActiveModes;
    timer_t m_Start;

public:
    void SetLogActiveMods(modname_t mod);

    void trace(modname_t mod, const char *fmt, ...);

    timer_t StartTimer();
    uint32_t EndTimer(timer_t tmr);
};

}
