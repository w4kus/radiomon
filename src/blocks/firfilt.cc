// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <volk/volk.h>
#include <cstring>
#include <stdio.h>

#include "firfilt.h"
#include "cmemset.h"
#include "cmove.h"

using namespace dsp;

firfilter::firfilter(const util::aligned_ptr<float> &taps)
{
    m_Taps = taps;
}

void firfilter::filter(const util::aligned_ptr<float> &inBlock, const util::aligned_ptr<float> &outBlock)
{
    auto *out = (float *)outBlock.get();

    if (m_fState.invalid())
    {
        m_fState = util::make_aligned_ptr<float>(m_Taps.size());
        std::memset((float *)m_fState.get(), 0, m_Taps.size() * sizeof(float));
    }

    auto *state = (float *)m_fState.get();

    for (size_t i=0;i < inBlock.size();i++)
    {
        std::memmove(&state[1], state, (m_fState.size() - 1) * sizeof(float));
        state[0] = inBlock[i];
        volk_32f_x2_dot_prod_32f(out, state, m_Taps.get(), m_Taps.size());
        ++out;
    }
}

void firfilter::filter(const util::aligned_ptr<std::complex<float>> &inBlock,
                        const util::aligned_ptr<std::complex<float>> &outBlock)
{
    auto *out = (std::complex<float> *)outBlock.get();

    if (m_cState.invalid())
    {
        m_cState = util::make_aligned_ptr<std::complex<float>>(m_Taps.size());
        util::cmemset<float>((std::complex<float> *)m_cState.get(), { 0.0f, 0.0f }, m_Taps.size());
    }

    auto *state = (std::complex<float> *)m_cState.get();

    for (size_t i=0;i < inBlock.size();i++)
    {
        util::cmove(&state[1], state, m_Taps.size() - 1);
        state[0] = inBlock[i];
        volk_32fc_32f_dot_prod_32fc(out, state, m_Taps.get(), m_Taps.size());
        ++out;
    }
}
