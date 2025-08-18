// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <memory>
#include <complex>

#include "firfilt.h"

namespace dsp {

/*! \brief Interpolation using a FIR filter.
 *
 * This implements linear interpolation using a FIR filter, supplied by
 * the user, and zero stuffing. Although not as efficient as a polyphase structure,
 * this can be useful with low tap filters since there's less algorithmic overhead.
 * Only real taps are supported.
 */

class firinterp
{
public:
    firinterp() = delete;
    firinterp(const firinterp &) = delete;
    firinterp& operator=(const firinterp &) = delete;

    //! Create an instance with a rational interpolation factor and FIR filter.
    //! @param [in] L       The rational interpolation facto
    //! @param [in] tapNum  The number of taps.
    //! @param [in] taps    The filter coefficients.
    firinterp(const uint32_t L, const size_t tapNum, const float *taps);

    //! Interpolate a block of a real signal.
    //! @param [in]     blkSize     The size of the block, e.g, the amount of floats.
    //! @param [in]     inBlock     A pointer to the block buffer.
    //! @param [out]    outBlock    A pointer to an output buffer of size **blkSize \* L**
    void filter(const size_t blkSize, const float *inBlock, const float *outBlock);

    //! Interpolate a block of a complex signal.
    //! @param [in]     blkSize     The size of the block, e.g., the amount of complex numbers.
    //! @param [in]     inBlock     A pointer to the block buffer.
    //! @param [out]    outBlock    A pointer to an output buffer of size **blkSize \* L**
    void filter(const size_t blkSize, const std::complex<float> *inBlock, const std::complex<float> *outBlock);

private:

    std::unique_ptr<firfilter> m_LpFilter;

    uint32_t m_L;
};

}
