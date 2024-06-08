#include <cstdint>
#include <stdarg.h>
#include <cstdio>
#include "log.h"

static uint8_t active_mods;

const char *mod_tags[] =
{
    "MAIN",
    "CORR"
};

#define TAG_SIZE    (sizeof(mod_tags) / sizeof(mod_tags[0]))

void dmr::SetLogActiveMods(modname_t mod)
{
    active_mods |= (uint8_t)mod;
}

void dmr::trace(modname_t mod, const char *fmt, ...)
{
    if (active_mods & mod)
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

            va_start(args, fmt);
            printf("[%s] ", mod_tags[tagIdx]);
            vprintf(fmt, args);
            va_end(args);
        }
    }
}
