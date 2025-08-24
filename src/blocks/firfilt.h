// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <stdlib.h>
#include <vector>
#include <complex>

#include "block.h"

namespace dsp {

/*! \brief Filter a signal with a FIR filter.
 *
 * This provides support for filtering a signal through a finite impulse response
 * (FIR) filter. The signal can be real or complex but only real coefficients are supported.
 * The non-transposed direct form (I/II) is used and any block size is acceptable. The state
 * of the filter is preserved for the life of the object instance.
 *
 * You can use an instance to filter both real and complex samples. Two state arrays are maintained
 * to facilitate this. The state arrays are created on the first call to one of the \ref filter
 * methods, so if you only use real or complex and not both, only one state array will be created.
 *
 * \note Typically the coefficents, or *taps*, are time reversed or *flipped* when used for filtering.
 * This implementation does **not** time reverse the taps.
 *
*/

class firfilter : public block
{
public:

    //! Create an instance for filtering.
    //! @param [in] taps    The array of coefficents.
    firfilter(const util::aligned_ptr<float> &taps);

    ~firfilter() { }

    //! Filter a segment of a real (float) signal.
    //! @param [in]  inBlock     The data to be filtered.
    //! @param [out] outBlock    The filtered data.
    void filter(const util::aligned_ptr<float> &inBlock, const util::aligned_ptr<float> &outBlock);

    //! Filter a segment of a complex signal.
    //! @param [in]  inBlock     The data to be filtered.
    //! @param [out] outBlock    The filtered data.
    void filter(const util::aligned_ptr<std::complex<float>> &inBlock,
                const util::aligned_ptr<std::complex<float>> &outBlock);

private:

    util::aligned_ptr<float> m_Taps;

    util::aligned_ptr<float> m_fState;

    util::aligned_ptr<std::complex<float>> m_cState;
};
}