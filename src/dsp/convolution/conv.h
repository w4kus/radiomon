// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <vector>

namespace dsp {

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