// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "freq-est.h"
#include "rm-math.h"

using namespace util;

float freq_est::estimate(const aligned_ptr<rm_math::complex_f> &in)
{
    // delayed sample vector
    auto v2 = make_aligned_ptr<rm_math::complex_f>(in.size());

    // result vector
    auto v3 = make_aligned_ptr<rm_math::complex_f>(in.size());

    // Run the data through the delay block to make the second vector
    for (size_t i=0;i < in.size();i++)
        v2[i] = m_Delay << in[i];

    // Do the math
    rm_math::mult_conj(&v3[0], in.get(), v2.get(), in.size());

    // Sum the result to average out the noise
    rm_math::complex_f sum = { 0.0f, 0.0f };
    for (size_t i=0;i < in.size();i++)
        sum += v3[i];

    // Finally determine the phase difference which is the current
    // frequency estimate.
    return m_Scale * rm_math::atan2(sum);
}
