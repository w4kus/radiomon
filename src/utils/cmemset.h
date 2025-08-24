// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <complex>

namespace util {

template<typename T>
void cmemset(std::complex<T> *dst, const std::complex<T> val, const size_t n)
{
    for (size_t i=0;i < n;i++)
    {
        dst[i].real(val.real());
        dst[i].imag(val.imag());
    }
}
}
