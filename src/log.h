#pragma once

#include <memory>
#include <chrono>
#include <mutex>
#include "timer.h"

#define TRACE dmr::log::getInst()->trace

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

    static auto &getInst()
    {
        static std::shared_ptr<log> p{ new log };
        return p;
    }

    log(const log&) = delete;
    log& operator=(const log&) = delete;

    void SetLogActiveMods(modname_t mod)
    {
        std::unique_lock<std::mutex> lck(m_Mtx);
        m_ActiveModes |= (uint8_t)mod;
    }

    void ClearLogActiveMods(modname_t mod)
    {
        std::unique_lock<std::mutex> lck(m_Mtx);
        m_ActiveModes &= (uint8_t)mod;
    }

private:

    log() : m_ActiveModes(0)
    {
        m_Start = util::timer::StartTimer();
    }

    uint8_t m_ActiveModes;
    util::timer::timer_t m_Start;
    std::mutex m_Mtx;

public:

    void trace(modname_t mod, const char *fmt, ...);
};
}
