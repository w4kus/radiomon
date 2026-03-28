// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <stdio.h>
#include <stdarg.h>

#include "aligned-ptr.h"

namespace util {

template<bool enable = false>
class trace
{
public:

    trace() { }
    trace(FILE *f) { }

    void print(const char *id, const char *fmt, ...)
    {
    }
};

template<>
class trace<true>
{
public:

    trace() : m_Fn { nullptr } { }
    trace(FILE *f) : m_Fn { f } { }

    trace(const trace &) = delete;
    trace& operator=(const trace &) = delete;

    trace(trace &&) = delete;
    trace& operator=(trace &&) = delete;

    ~trace()
    {
        if (m_Fn) fclose(m_Fn);
    }

    void print(const char *id, const char *fmt, ...)
    {
        va_list args;

        printf("[%s]: ", id);

        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);

        if (m_Fn)
        {
            fprintf(m_Fn, "[%s]: ", id);

            va_start(args, fmt);
            vfprintf(m_Fn, fmt, args);
            va_end(args);
        }
    }

private:

    FILE *m_Fn;
};
}
