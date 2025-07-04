// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <vector>

namespace dsp {

/*! \brief Template for time domain convolution.
 *
 * This calculates the convolution of two sequences.  Unlike the conv_full implementation
 * template, this skips the zero padded edges.  Therefore, given
 * two sequences **b**, of size **m**, and **h**, of size **n**, this
 * produces an output of size m - n + 1.  This implies that **m** must be greater
 * than or equal to **n**.  This is what the *Octave/Matlab* function *conv*
 * returns when given the 'valid' option.
 *
 * The output is of type *std::vector<T>*.  It is expected that **T**
 * be either a floating point type (e.g. *float*, *double*) or *std::complex<T>*
 * but no restrctions are placed on it.  If the size relation for **m** and **n** isn't
 * satisfied, this will return a zero length vector.
 *
*/


template<typename T>
class conv
{
public:

    std::vector<T> convolve(const T *b, const size_t m, const T *h, const size_t n)
    {
        int len = m - n + 1;

        if (len <= 0)
            return std::vector<T>(0);

        size_t k = n + 1;
        size_t min;

        std::vector<T> res(len);

        for (size_t i=0;i < (size_t)len;i++,k++)
        {
            res[i] = 0;
            min = k - n + 1;
            for (size_t j=0;j < n; j++)
                res[i] += b[min+j] * h[j];
        }

        return res;
    }
};

}