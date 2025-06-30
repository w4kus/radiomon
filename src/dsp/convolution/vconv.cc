// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <volk/volk.h>
#include <cstring>
#include <vconv.h>

std::vector<float> dsp::vconvolve(const float *b, const size_t m, const float *h, const size_t n)
{
    int len = m - n + 1;

    if (len <= 0)
        return std::vector<float>(0);

    size_t k = n + 1;
    size_t min;

    std::vector<float> res(len);

    float *aligned_b_buff = (float *)volk_malloc(m * sizeof(float), volk_get_alignment());
    float *aligned_h_buff  = (float *)volk_malloc(n * sizeof(float), volk_get_alignment());

    std::memcpy(aligned_h_buff, h, n * sizeof(float));
    std::memcpy(aligned_b_buff, b, m * sizeof(float));

    for (size_t i=0;i < (size_t)len;i++, k++)
    {
        res[i] = 0;
        min = k - n + 1;
        volk_32f_x2_dot_prod_32f(&res[i], &aligned_b_buff[min], aligned_h_buff, n);
    }

    volk_free(aligned_b_buff);
    volk_free(aligned_h_buff);

    return res;
}

std::vector<std::complex<float>> dsp::vconvolve(const std::complex<float> *b, const size_t m, const float *h, const size_t n)
{
    int len = m - n + 1;

    if (len <= 0)
        return std::vector<std::complex<float>>(0);

    size_t k = n + 1;
    size_t min;

    std::vector<std::complex<float>> res(len);

    std::complex<float> *aligned_b_buff = (std::complex<float> *)volk_malloc(m * sizeof(std::complex<float>), volk_get_alignment());
    float *aligned_h_buff = (float *)volk_malloc(n * sizeof(float), volk_get_alignment());

    std::memcpy(aligned_h_buff, h, n * sizeof(float));
    std::memcpy(aligned_b_buff, b, m * sizeof(std::complex<float>));

    for (size_t i=0;i < (size_t)len;i++, k++)
    {
        res[i] = 0;
        min = k - n + 1;
        volk_32fc_32f_dot_prod_32fc(&res[i], &aligned_b_buff[min], aligned_h_buff, n);
    }

    volk_free(aligned_b_buff);
    volk_free(aligned_h_buff);

    return res;
}

