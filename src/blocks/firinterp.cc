// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <cassert>
#include <cstring>

#include "firinterp.h"

using namespace dsp;

firinterp::firinterp(const uint32_t L, const util::aligned_ptr<float> &taps, const bool adjustGain) :
    m_L(L)
{
    assert(L > 0);

    if (adjustGain)
    {
        float *p = (float *)taps.get();
        for (size_t i=0;i < taps.size();i++)
            p[i] *= (float)L;
    }

    m_LpFilter = std::make_unique<firfilter>(taps);
}

void firinterp::filter(const util::aligned_ptr<float> &inBlock, const util::aligned_ptr<float> &outBlock)
{
    const size_t outBlockSize = inBlock.size() * m_L;
    auto filterBlock = util::make_aligned_ptr<float>(outBlockSize);
    auto *p = (float *)filterBlock.get();

    for (size_t i=0,j=0;i < inBlock.size();i++)
    {
        p[j++] = inBlock[i];

        uint32_t cnt = m_L - 1;
        while(cnt--)
            p[j++] = 0.0f;
    }

    m_LpFilter->filter(filterBlock, outBlock);
}

void firinterp::filter(const util::aligned_ptr<std::complex<float>> &inBlock, const util::aligned_ptr<std::complex<float>> &outBlock)
{
    const size_t outBlockSize = inBlock.size() * m_L;
    auto filterBlock = util::make_aligned_ptr<std::complex<float>>(outBlockSize);
    auto *p = (std::complex<float> *)filterBlock.get();

    for (size_t i=0,j=0;i < inBlock.size();i++)
    {
        p[j++] = inBlock[i];

        uint32_t cnt = m_L - 1;
        while(cnt--)
        {
            p[j].real(0.0f);
            p[j++].imag(0.0f);
        }
    }

    m_LpFilter->filter(filterBlock, outBlock);
}
