// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <complex>
#include <volk/volk.h>
#include <cmath>
#include <type_traits>

// See the 'using' statement at the bottom to set which library to use; default
// is Volk.

//////////////////////////////////////////////////////////////////////////////
// Volk specializations
namespace util {

template<typename T>
struct is_std_complex : std::false_type { };

template<typename T>
struct is_std_complex<std::complex<T>> : std::true_type { };

template<typename T>
constexpr bool is_std_complex_v = is_std_complex<std::complex<T>>::value;

struct volk
{
    using complex_f = std::complex<float>;

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

    // real dot product
    static void dot_prod(float *out, const float *in, const float *taps, unsigned int num_points)
    {
        volk_32f_x2_dot_prod_32f(out, in, taps, num_points);
    }

    // complex dot product
    static void dot_prod(std::complex<float> *out, const std::complex<float> *in, const float *taps, unsigned int num_points)
    {
        volk_32fc_32f_dot_prod_32fc(out, in, taps, num_points);
    }

    // block cosine
    static void blk_cos(float *out, const float *in, unsigned int num_points)
    {
        volk_32f_cos_32f(out, in, num_points);
    }

    // block sine
    static void blk_sin(float *out, const float *in, unsigned int num_points)
    {
        volk_32f_sin_32f(out, in, num_points);
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

    // multiply two vectors
    static void vect_mult(float *out, const float *v1, const float *v2, int num_points)
    {
        volk_32f_x2_multiply_32f(out, v1, v2, num_points);
    }

    // multiply a vector with a scaler
    static void vect_scaler_mult(float *out, const float *v1, const float s, int num_points)
    {
        volk_32f_s32f_multiply_32f(out, v1, s, num_points);
    }

    // add a vector with a scaler
    static void vect_scaler_add(float *out, const float *v1, const float s, int num_points)
    {
        volk_32f_s32f_add_32f(out, v1, s, num_points);
    }

    // float rounding to a specified number of fractional digits
    static float round(float value, uint8_t digits);
};

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

    // block sine
    static void blk_sin(float *out, const float *in, unsigned int num_points)
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

    // multiply two vectors
    static void vect_mult(float *out, const float *v1, const float *v2, int num_points)
    {
    }

    // multiply a vector with a scaler
    static void vect_scaler_mult(float *out, const float *v1, const float s, int num_points)
    {
    }

    // add a vector with a scaler
    static void vect_scaler_add(float *out, const float *v1, const float s, int num_points)
    {
    }
};

}

// Set which library to use:
// * util::volk
// * util::cmsisdsp (future)
using rm_math = util::volk;
