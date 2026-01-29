// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include "delay.h"
#include "aligned-ptr.h"

namespace comps {

/*! \brief Numerically Controlled Ocillator
 *
 * Complex signals are generated; the caller can 
 * discard one of the components if not needed.
 */

class nco
{
public:

    nco() = delete;

    //! Create an instance.
    //! @param [in] K   The input gain. In general, this is typically set to one.
    nco(float K) : m_K { K }, m_Phase { 0.0f }, m_Freq { 0.0f }
    {
    }

    //! Update the input gain.
    //! @param [in] gain    The new (linear) gain.
    void setGain(float gain)
    {
        m_K = gain;
    }

    //! Update the frequency.
    //! @param [in] freq    The frequency in radians per sample.
    void setFrequency(float freq)
    {
        m_Freq = freq;
    }

    //! Adjust the phase of the generated signal through the phase accumulator.
    //! @param [in] error   The amount to adjust the phase in radians.
    //! @return The associated complex sample adjusted by the given error in radians per sample.
    rm_math::complex_f adjustPhase(float error);

private:

    float m_K;
    float m_Phase;
    float m_Freq;
    delay<float> m_Delay;
};

}
