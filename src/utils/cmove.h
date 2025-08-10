// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <complex>

namespace util {

template<typename T>
void cmove(std::complex<T> *dst, const std::complex<T> *src, size_t n)
{
    if (dst < src)
    {
        for (size_t i=0;i < n;i++)
        {
            dst[i].real(src[i].real());
            dst[i].imag(src[i].imag());
        }
    }
    else
    {
        for (size_t i=n; i > 0;i--)
        {
            dst[i-1].real(src[i-1].real());
            dst[i-1].imag(src[i-1].imag());
        }
    }
}
}
