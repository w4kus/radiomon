#pragma once

namespace dmr {

typedef enum
{
    MAIN = 1,
    CORR = 2,

    ALL = MAIN | CORR
}modname_t;

void SetLogActiveMods(modname_t mod);

void trace(modname_t mod, const char *fmt, ...);

}
