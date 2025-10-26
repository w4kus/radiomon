// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <complex>
#include <volk/volk.h>
#include <cmath>
#include "cmemset.h"

// See the 'using' statement at the bottom to set which library to use; default
// is Volk.

//////////////////////////////////////////////////////////////////////////////
// Volk specializations
namespace util {

struct volk
{
    // memory
    template<typename T>
    static T* rm_malloc(size_t size)
    {
        return (T *)volk_malloc(size * sizeof(T), volk_get_alignment());
    }

    static inline void rm_free(void *ptr)
    {
        volk_free(ptr);
    }

    // dot product
    static void dot_prod(float *out, const float *in, const float *taps, unsigned int num_points)
    {
        volk_32f_x2_dot_prod_32f(out, in, taps, num_points);
    }

    static void dot_prod(std::complex<float> *out, const std::complex<float> *in, const float *taps, unsigned int num_points)
    {
        volk_32fc_32f_dot_prod_32fc(out, in, taps, num_points);
    }

    // block cosine
    static void blk_cos(float *out, const float *in, unsigned int num_points)
    {
        volk_32f_cos_32f(out, in, num_points);
    }

    // sine
    static float sin(float num)
    {
        return std::sin(num);
    }

    // cosine
    static float cos(float num)
    {
        return std::cos(num);
    }

    // multiply a complex vector with the conjugate of another complex vector; this
    // routine conjugates the 'conjIn' vector.
    static void mult_conj(std::complex<float> *out, const std::complex<float> *in,
                            const std::complex<float> *conjIn, unsigned int num_points)
    {
        volk_32fc_x2_multiply_conjugate_32fc(out, in, conjIn, num_points);
    }

    // atan2
    static float atan2(std::complex<float> in)
    {
        return std::atan2(in.imag(), in.real());
    }
};

}

//////////////////////////////////////////////////////////////////////////////
// ARM Cortex-M (future)
struct cmsis
{
    template<typename T>
    static T* rm_malloc(size_t size)
    {
    }

    static void rm_free(void *ptr)
    {
    }

    // dot product
    static void dot_prod(float *out, const float *in, const float *taps, unsigned int num_points)
    {
    }

    static void dot_prod(std::complex<float> *out, const std::complex<float> *in, const float *taps, unsigned int num_points)
    {
    }

    // block cosine
    static void blk_cos(float *out, const float *in, unsigned int num_points)
    {
    }

    // sine
    static float sin(float num)
    {
        return 0.0f;
    }

    // cosine
    static float cos(float num)
    {
        return 0.0f;
    }

    // multiply a complex vector with the conjugate of another complex vector
    static void mult_conj(std::complex<float> *out, const std::complex<float> *in,
                            const std::complex<float> *conjIn, unsigned int num_points)
    {
    }
};

// Set which library to use:
// * util::volk
// * util::cmsisdsp (future)
using rm_math = util::volk;
