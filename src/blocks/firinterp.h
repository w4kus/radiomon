// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <memory>
#include <complex>

#include "aligned_ptr.h"
#include "firfilt.h"

namespace dsp {

/*! \brief Interpolation using a FIR filter.
 *
 * This implements linear interpolation using a FIR filter, supplied by
 * the user, and zero stuffing. Although not as efficient as a polyphase structure,
 * this can be useful with low tap filters since there's less algorithmic overhead.
 * Only real taps are supported.
 */

class firinterp : public block
{
public:

    //! Create an instance with a rational interpolation factor and FIR filter.
    //! @param [in] L       The rational interpolation factor
    //! @param [in] taps    The filter coefficients.
    //! @param [in] adjustGain  Adjust the coeffcients by *L*. Defaults to *true*.
    firinterp(const uint32_t L, const util::aligned_ptr<float> &taps, const bool adjustGain = true);

    //! Interpolate a block of a real signal.
    //! @param [in]     inBlock     A pointer to the block buffer.
    //! @param [out]    outBlock    A pointer to an output buffer of size **blkSize \* L**
    void filter(const util::aligned_ptr<float> &inBlock, const util::aligned_ptr<float> &outBlock);

    //! Interpolate a block of a complex signal.
    //! @param [in]     inBlock     A pointer to the block buffer.
    //! @param [out]    outBlock    A pointer to an output buffer of size **blkSize \* L**
    void filter(const util::aligned_ptr<std::complex<float>> &inBlock, const util::aligned_ptr<std::complex<float>> &outBlock);

private:

    std::unique_ptr<firfilter> m_LpFilter;

    uint32_t m_L;
};

}
