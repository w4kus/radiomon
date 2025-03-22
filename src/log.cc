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

            std::unique_lock<std::mutex> lck(m_Mtx);

            auto dur = util::timer::EndTimer(m_Start);

            va_start(args, fmt);
            printf("<%u>[%s] ", dur, mod_tags[tagIdx]);
            vprintf(fmt, args);
            va_end(args);
        }
    }
}
