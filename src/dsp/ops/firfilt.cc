// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <volk/volk.h>
#include <cstring>

#include "firfilt.h"
#include "cmemset.h"
#include "cmove.h"

using namespace dsp;

firfilter::firfilter(const size_t tapNum, const float *taps) :
                    m_TapNum(tapNum),
                    m_Taps(nullptr),
                    m_fState(nullptr),
                    m_cState(nullptr)
{
    m_Taps = (float *)volk_malloc(m_TapNum * sizeof(float), volk_get_alignment());
    std::memcpy(m_Taps, taps, m_TapNum * sizeof(float));
}

firfilter::~firfilter()
{
    volk_free(m_Taps);

    if (m_fState) volk_free(m_fState);
    if (m_cState) volk_free(m_cState);
}

void firfilter::filter(const size_t blkSize, const float *inBlock, const float *outBlock)
{
    auto *out = (float *)outBlock;

    if (!m_fState)
    {
        m_fState = (float *)volk_malloc(m_TapNum * sizeof(float), volk_get_alignment());
        std::memset(m_fState, 0, m_TapNum * sizeof(float));
    }

    for (size_t i=0;i < blkSize;i++)
    {
        std::memmove(&m_fState[1], m_fState, (m_TapNum - 1) * sizeof(float));
        m_fState[0] = inBlock[i];
        volk_32f_x2_dot_prod_32f(out, m_fState, m_Taps, m_TapNum);
        ++out;
    }
}

void firfilter::filter(const size_t blkSize, const std::complex<float> *inBlock, const std::complex<float> *outBlock)
{
    auto *out = (std::complex<float> *)outBlock;

    if (!m_cState)
    {
        m_cState = (std::complex<float> *)volk_malloc(m_TapNum * sizeof(std::complex<float>), volk_get_alignment());
        util::cmemset(m_cState, std::complex<float>{0, 0}, m_TapNum);
    }

    for (size_t i=0;i < blkSize;i++)
    {
        util::cmove(&m_cState[1], m_cState, m_TapNum);
        m_cState[0] = inBlock[i];
        volk_32fc_32f_dot_prod_32fc(out, m_cState, m_Taps, m_TapNum);
        ++out;
    }
}
