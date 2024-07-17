#include <cstdint>
#include <stdarg.h>
#include <cstdio>
#include <iostream>
#include "log.h"

const char *mod_tags[] =
{
    "MAIN",
    "CORR"
};

#define TAG_SIZE    (sizeof(mod_tags) / sizeof(mod_tags[0]))

using namespace dmr;

void log::SetLogActiveMods(modname_t mod)
{
    m_ActiveModes |= (uint8_t)mod;
}

void log::trace(modname_t mod, const char *fmt, ...)
{
    if (m_ActiveModes & mod)
    {
        uint8_t bit = mod;
        uint8_t tagIdx;
        uint8_t j;
        for (tagIdx=0, j=1;tagIdx < TAG_SIZE;tagIdx++, j<<=1)
        {
            if (j & bit)
                break;
        }

        if (tagIdx < TAG_SIZE)
        {
            va_list args;

            auto tick = std::chrono::steady_clock::now();
            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(tick - m_Start);

            va_start(args, fmt);
            printf("<%u>[%s] ", (uint32_t)dur.count(), mod_tags[tagIdx]);
            vprintf(fmt, args);
            va_end(args);
        }
    }
}

log::timer_t log::StartTimer()
{
    return std::chrono::steady_clock::now();
}

uint32_t log::EndTimer(log::timer_t tmr)
{
    auto now = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(now - tmr).count();
}
