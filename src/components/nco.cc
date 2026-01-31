// Copyright (c) 2026 John Mark nWhite -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "nco.h"

using namespace comps;

rm_math::complex_f nco::adjustPhase(float error)
{
    m_Phase = m_Delay << m_Freq + (error * m_K) + m_Phase;

    if (m_Phase > (2 * M_PI))
        m_Phase -= (2 * M_PI);

    return { rm_math::cos(m_Phase), rm_math::sin(m_Phase) };
}
