// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include "aligned-ptr.h"
#include "delay.h"
#include "rm-math.h"

namespace util {

/*! \brief Frequency Estimater
 *
 * Given a block of a complex signal, this will estimate the frequency of
 * that signal within the given block. It uses the delay / multiply technique
 * which relies on the phase difference between successive samples. This can
 * be incorporated into a frequency lock loop (FLL) as the frequency error
 * detector (FED) since it provides a quick estimate which can then be sent
 * to, e.g., a PLL.
 */

class freq_est
{
public:
    freq_est() = delete;

    //! Create an instance of the estimater.
    //! @param [in] scale   The value to use to scale the result. Some possible values:
    //!     - **1.0** (no scaling) returns a value in radians.
    //!     - **2*PI/Fs** where Fs is the sampling rate, returns a value in radians / sample.
    //!     - **Fs/2*pi** returns a value in Hz.
    freq_est(float scale) : m_Scale { scale } { }

    //! Estimate a block of samples.
    //! @param [in] in      The block of samples with which to make an estimation.
    //!
    //! @return The scaled estimate.
    float estimate(const aligned_ptr<rm_math::complex_f> &in);

    //! Update the scaling factor.
    //! @param [in] scale   The new scaling factor.
    void setScale(float scale) { m_Scale = scale; }

private:
    float m_Scale;
    delay<rm_math::complex_f> m_Delay;
};

}
