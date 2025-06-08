#pragma once

#include <vector>

namespace dsp {

template<typename T>
class conv
{
public:

    std::vector<T> convolve(const T *b, const uint32_t m, const T *h, const uint32_t n)
    {
        int len = m - n + 1;

        if (len <= 0)
            return std::vector<T>(0);

        size_t k = n - 1;
        size_t max;
        size_t min;
        std::vector<T> res(len);

        for (size_t i=0;i < (size_t)len;i++,k++)
        {
            res[i] = 0;
            
            min = (k >= (n - 1)) ? k - (n - 1) : 0;
            max = (k < (m - 1)) ? k : (m - 1);

            for (size_t j=min;j <= max;j++)
                res[i] += b[j] * h[k - j];
        }

        return res;
    }
};

}