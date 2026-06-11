// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <algorithm>
#include <vector>
#include <array>

#include "block.h"

namespace dsp {

/*! \brief Filter a signal with an FIR filter.
 *
 * This provides support for filtering a signal through a finite impulse response
 * (FIR) filter. The signal can be real or complex but only real coefficients are supported.
 *
*/

template<typename T, typename B>
class firfilter : public block<B>
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert(is_block_func_v<B>);

public:
    //! Create an instance for filtering.
    //! @param [in] taps    An aligned_ptr of coefficents.
    firfilter(const util::aligned_ptr<float> &taps) : block<B> { TYPE_OPERATOR }, m_Taps { taps }
    {
        block<B>::process = std::bind(&firfilter::filter, this, std::placeholders::_1, std::placeholders::_2);
        util::init_aligned_ptr<T>(m_State, m_Taps.size());
    }

    //! Create an instance for filtering.
    //! @param [in] taps    A vector of coefficents.
    firfilter(const std::vector<float> &taps) : block<B> { TYPE_OPERATOR }
    {
        block<B>::process = std::bind(&firfilter::filter, this, std::placeholders::_1, std::placeholders::_2);
        util::init_aligned_ptr<float>(m_Taps, taps.size(), taps.data());
        util::init_aligned_ptr<T>(m_Taps, taps.size());
    }

    //! Create an instance for filtering.
    //! @param [in] taps    An array of coefficents. 
    template<size_t S>
    firfilter(const std::array<float, S> &taps) : block<B> { TYPE_OPERATOR }
    {
        block<B>::process = std::bind(&firfilter::filter, this, std::placeholders::_1, std::placeholders::_2);
        util::init_aligned_ptr<float>(m_Taps, taps.size(), taps.data());
        util::init_aligned_ptr<T>(m_Taps, taps.size());
    }

    //! Filter a segment of a signal.
    //! @param [in]  inBlock     The data to be filtered.
    //! @param [out] outBlock    The filtered data.
    void filter(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        util::init_aligned_ptr_on_resize<T>(outBlock, inBlock.size());

        for (size_t i=0;i < inBlock.size();i++)
        {
            std::move_backward(m_State.begin(), m_State.end() - 1, m_State.end());
            m_State[0] = inBlock[i];
            rm_math::dot_prod(&outBlock[i], m_State.data(), m_Taps.data(), m_Taps.size());
        }
    }

private:

    util::aligned_ptr<float> m_Taps;
    util::aligned_ptr<T> m_State;
};

using firfilter_ff = firfilter<float, dsp::func_ff>;
using firfilter_cc = firfilter<rm_math::complex_f, dsp::func_cc>;

}
