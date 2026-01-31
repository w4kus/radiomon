// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "carrier-sync.h"
#include "rm-math.h"

#include <stdio.h>

using namespace dsp;

carrier_sync::carrier_sync(const float Kp, const float Ki, const size_t errorPortSize) :
                            m_LoopFilt { Kp, Ki },
                            m_Nco { M_PI / 16 },
                            m_Est {1.0f },
                            m_ErrorPort { errorPortSize }
{
    block<func_cc>::process = std::bind(&carrier_sync::sync, this, std::placeholders::_1, std::placeholders::_2);

    // m_Tick = util::timer::StartTimer();
}

void carrier_sync::sync(const util::aligned_ptr<rm_math::complex_f> &inBlock, util::aligned_ptr<rm_math::complex_f> &outBlock)
{
    auto errorSig = util::make_aligned_ptr<float>(inBlock.size());

    outBlock = util::make_aligned_ptr<rm_math::complex_f>(inBlock.size());

    // Update the estimater scaling factor using the current sampling rate but only
    // if the sampling rate has changed (prevent unnecessary division).
    if (m_CurrentSampleRate != m_SamplingRate)
    {   
        m_CurrentSampleRate = m_SamplingRate;
        m_Est.setScale(2 * M_PI / (float)m_SamplingRate);
    }

    // Get the estimate and update the NCO
    m_Nco.setFrequency(m_Est.estimate(inBlock));

    // PLL to fine tune the estimated frequency and also good for handling noisy channels
    for (size_t i = 0; i < inBlock.size(); i++)
    {
        // Get the sample adjusted by the error and store it in the
        // output buffer
        outBlock[i] = m_Nco.adjustPhase(m_LoopFilt.filter(m_Error));

        // Calculate the new error
        m_Error = rm_math::atan2(inBlock[i] * std::conj(outBlock[i]));
        errorSig[i] = m_Error;
    }

    // Make the error signal available for testing
    m_ErrorPort.produce(std::move(errorSig));

#if 0
    if (util::timer::EndTimer(m_Tick) > 1000)
    {
        fprintf(m_TestFile, "%d %0.6f\n", uint32_t(est), m_Error);
        m_Tick = util::timer::StartTimer();
    }
#endif
}
