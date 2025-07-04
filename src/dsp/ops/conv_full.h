// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
// 
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <vector>

namespace dsp {

/*! \brief Template for time domain convolution.
 *
 * This calculates the standard convolution of two sequences.  Given
 * two sequences **b**, of size **m**, and **h**, of size **n**, this
 * produces an output of size m + n - 1.
 *
 * The output is of type *std::vector<T>*.  It is expected that **T**
 * be either a floating point type (e.g. *float*, *double*) or *std::complex<T>*
 * but no restrctions are placed on it.
 *
 */


template<typename T>
class conv_full
{
public:

    std::vector<T> convolve(const T *b, const size_t m, const T *h, const size_t n)
    {
        size_t len = m + n - 1;
        size_t min;
        size_t max;
        std::vector<T> res(len);

        for (size_t i=0;i < len;i++)
        {
            res[i] = 0;
            
            min = (i >= (n - 1)) ? i - (n - 1) : 0;
            max = (i < (m - 1)) ? i : (m - 1);

            for (size_t j=min;j <= max;j++)
                res[i] += b[j] * h[i - j];
        }
        
        return res;
    }
};

}
