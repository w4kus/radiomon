// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "loop-filter.h"

using namespace comps;

float loopfilt::filter(float value)
{
    // PI filter, forward integral
    float ei = (m_Ki * value) + m_Delay.get();
    m_Delay << ei;
    
    return (m_Kp * value) + ei;
}
