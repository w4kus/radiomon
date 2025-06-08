#pragma once

#include <vector>

namespace dsp {

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
