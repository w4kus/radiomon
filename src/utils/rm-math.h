// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <cassert>
#include <complex>
#include <volk/volk.h>
#include <cstring>
#include "cmemset.h"

// See the 'using' statement at the bottom to set which library to use; default
// is Volk.

//////////////////////////////////////////////////////////////////////////////
// Volk specializations
namespace util {
    namespace math {
        namespace volk {

// dot product
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
}}}

//////////////////////////////////////////////////////////////////////////////
// ARM Cortex-M specializations (future)
namespace util {
    namespace math {
        namespace cmsisdsp {


}}}

// Set which library namespace to use:
// * util::math::volk
// * util::math::cmsisdsp (future)
using namespace util::math::volk;
