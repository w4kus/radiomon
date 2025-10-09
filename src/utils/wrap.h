// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <cassert>
#include <complex>
#include <volk/volk.h>
#include <cstring>
#include "cmemset.h"

namespace util {

//////////////////////////////////////////////////////////////////////////////
// Volk dot product specializations
template<typename T, typename U, typename V>
void dot_prod(T *out, const U *in, const V *taps, unsigned int num_points)
{
}

template<>
inline void dot_prod(float *out, const float *in, const float *taps, unsigned int num_points)
{
    volk_32f_x2_dot_prod_32f(out, in, taps, num_points);
}

template<>
inline void dot_prod(std::complex<float> *out, const std::complex<float> *in, const float *taps, unsigned int num_points)
{
    volk_32fc_32f_dot_prod_32fc(out, in, taps, num_points);
}

}
