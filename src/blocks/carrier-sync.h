// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include "block.h"
#include "aligned-ptr.h"
#include "loop-filter.h"
#include "nco.h"
#include "freq-est.h"
#include "port.h"

#include "timer.h"
#include <stdio.h>

namespace dsp {

/*! \brief Carrier Phase Synchronizer
 *
 * Given a block of complex samples, this does the following:
 * 1. Estimates the frequency.
 * 2. Sets a *numerically controlled oscillator* (NCO) to the estimated frequency.
 * 3. Passes the estimated samples through a *phased lock loop* (PLL) for fine tuning.
 * 
 * The result is a sinusoid which is phased synchronized with the estimated frequecy of
 * the down-sampled passband carrier. This is needed for coherent modulation techniques
 * such as *QAM* with its derivities (e.g., *M-PSK)*. Non-coherent modulations like *FSK* 
 * and *DPSK* generally should avoid using this block since the carrier phase is not needed.
*/

class carrier_sync : public block<func_cc>
{
public:

    //! Create an instance of the carrier synchronizer.
    //! @param [in] Kp              The proportional constant of the NCO. See the *loop-filter* documention for details.
    //! @param [in] Ki              The integral constant of the NCO. See the *loop-filter* documention for details.
    //! @param [in] errorPortSize   The size of the PLL error buffer. This will contain the
    //!                             error calculations of the PLL which and be used for
    //!                             testing and debugging. See the *port* documention for details.
    //!                             By default, this is set to zero which does not generate an error signal.
    carrier_sync(const float Kp, const float Ki, const size_t errorPortSize = 0);

    //! Generate a sinusoid fragment from the input samples which is frequency and phase aligned with the
    //! input samples.
    //! @param [in]     inBlock The received samples.
    //! @param [out]    outBlock The generated samples. *outBlock* shall be empty.
    void sync(const util::aligned_ptr<rm_math::complex_f> &inBlock, util::aligned_ptr<rm_math::complex_f> &outBlock);

    //! Get the current buffer of error calculations.
    //! @param [out] samples The current buffer of PLL error calculations. *samples* shall be empty.
    void getErrorSig(util::aligned_ptr<float> &samples)
    {
        m_ErrorPort.consume(samples);
    }
    
private:

    comps::loopfilt  m_LoopFilt;
    comps::nco       m_Nco;
    comps::freq_est  m_Est;

    uint32_t m_CurrentSampleRate;
    float m_Error;

    util::port<float> m_ErrorPort;

    util::timer::timer_t m_Tick;
    FILE *m_TestFile;
};
}
