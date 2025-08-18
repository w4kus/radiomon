// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <cassert>

#include "firinterp.h"

using namespace dsp;

firinterp::firinterp(const uint32_t L, const size_t tapNum, const float *taps) :
    m_L(L)
{
    assert(L > 0);

    m_LpFilter = std::make_unique<firfilter>(tapNum, taps);
}

void firinterp::filter(const size_t blkSize, const float *inBlock, const float *outBlock)
{
    const size_t outBlockSize = blkSize * m_L;
    auto filterBlock = std::make_unique<float[]>(outBlockSize);

    for (size_t i=0,j=0;i < blkSize;i++)
    {
        filterBlock[j++] = inBlock[i];

        uint32_t cnt = m_L - 1;
        while(cnt--)
            filterBlock[j++] = 0.0f;
    }

    m_LpFilter->filter(outBlockSize, filterBlock.get(), outBlock);
}

void firinterp::filter(const size_t blkSize, const std::complex<float> *inBlock, const std::complex<float> *outBlock)
{
    const size_t outBlockSize = blkSize * m_L;

    auto filterBlock = std::make_unique<std::complex<float>[]>(outBlockSize);
    for (size_t i=0,j=0;i < blkSize;i++)
    {
        filterBlock[j++] = inBlock[i];

        uint32_t cnt = m_L - 1;
        while(cnt--)
        {
            filterBlock[j].real(0.0f);
            filterBlock[j++].imag(0.0f);
        }
    }

    m_LpFilter->filter(outBlockSize, filterBlock.get(), outBlock);
}
