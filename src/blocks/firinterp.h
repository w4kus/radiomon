// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <memory>
#include <complex>

#include "block.h"
#include "aligned_ptr.h"
#include "firfilt.h"
#include "wrap.h"

namespace dsp {

/*! \brief Interpolation using a FIR filter.
 *
 * This implements interpolation using a FIR filter, supplied by
 * the user, and zero stuffing. Only real taps are supported.
 */

template<typename T, typename B>
class firinterp : public block<B>
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert(is_block_func_v<B>);

public:

    //! Create an instance with a integer interpolation factor and FIR filter.
    //! @param [in] L       The integer interpolation factor
    //! @param [in] taps    The filter coefficients.
    //! @param [in] adjustGain  Adjust the coeffcients by *L*. Defaults to *true*.
    firinterp(const uint16_t L, const util::aligned_ptr<float> &taps, const bool adjustGain = true) :
                m_L { L }
    {
        assert(L > 0);

        block<B>::process = std::bind(&firinterp::interp, this, std::placeholders::_1, std::placeholders::_2);

        if (adjustGain)
        {
            for (auto it = taps.begin(); it != taps.end();++it)
                *it *= L;
        }

        m_LpFilter = std::make_unique<firfilter<T, B>>(taps);
    }

    //! Interpolate a block of a signal.
    //! @param [in]     inBlock     The data to be interpolated.
    //! @param [out]    outBlock    The interpoldated data which will be the size of *inBlock* * **L**.
    void interp(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        const size_t outBlockSize = inBlock.size() * m_L;

        auto filterBlock = util::make_aligned_ptr<T>(outBlockSize);
        outBlock = util::make_aligned_ptr<T>(outBlockSize);

        const uint16_t cnt = m_L - 1;

        for (auto it=inBlock.begin(), jt=filterBlock.begin();it != inBlock.end();++it)
        {
            *jt = *it;
            ++jt;
            std::fill(jt, jt+cnt, 0);
            jt += cnt;
        }

        m_LpFilter->filter(filterBlock, outBlock);
    }

private:

    uint16_t m_L;
    std::unique_ptr<firfilter<T, B>> m_LpFilter;
};

using firinterp_ff = firinterp<float,dsp::func_ff>;
using firinterp_cc = firinterp<std::complex<float>,dsp::func_cc>;

}
