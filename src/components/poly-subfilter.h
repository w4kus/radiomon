// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <vector>
#include <array>

#include "aligned-ptr.h"

namespace comps {

/*! \brief Polyphase Sub-filter
 *
 * Create a sub-filter of a polyphase filter structure. This is essentially the *firfilt* block
 * but with the loops unrolled. Each branch of a polyphase filter structure consists of a
 * portion of the coefficients of a complete FIR filter.
 */

template<typename T>
class poly_subfilter
{
    static_assert(util::is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));

public:

    //! Create an instance using a const C array of type T
    //! @param [in] numTaps The number of taps in the sub-filter.
    //! @param [in] taps    The coefficients of the sub-filter.
    poly_subfilter(const size_t numTaps, const float *taps)
    {
        util::init_aligned_ptr<T>(m_State, numTaps);
        util::init_aligned_ptr<float>(m_Taps, numTaps, taps);
        std::memset(&m_State[0], 0, numTaps * sizeof(T));
    }

    //! Create an instance using a const vector of type T
    //! @param [in] taps    The coefficients of the sub-filter.
    poly_subfilter(const std::vector<float> &taps)
    {
        util::init_aligned_ptr<T>(m_State, taps.size());
        util::init_aligned_ptr<float>(m_Taps, taps.size(), taps.data());
        std::memset(&m_State[0], 0, taps.size() * sizeof(T));
    }

    //! Create an instance using a const std::array of type T and size S
    //! @param [in] taps    The coefficients of the sub-filter.
    template<size_t S>
    poly_subfilter(const std::array<float, S> &taps)
    {
        util::init_aligned_ptr<T>(m_State, taps.size());
        util::init_aligned_ptr<float>(m_Taps, taps.size(), taps.data());
        std::memset(&m_State[0], 0, taps.size() * sizeof(T));
    }

    //! Insert a new sample into the delay line and return the result.
    //! @param [in] sample The sample to insert.
    //! @return The new filtered sample.
    T insert(const T sample)
    {
        T out;

        std::move_backward(m_State.begin(), m_State.end() - 1, m_State.end());
        m_State[0] = sample;
        rm_math::dot_prod(&out, m_State.data(), m_Taps.data(), m_Taps.size());

        return out;
    }

    //! Allow copying for building polyphase structures.
    poly_subfilter(const poly_subfilter &other)
    {
        if (this == &other)
            return;

        m_Taps = other.m_Taps;
        m_State = other.m_State;
    }

    //! Allow copying for building polyphase structures.
    poly_subfilter& operator=(const poly_subfilter &other)
    {
        if (this == &other)
            return *this;

        m_Taps = other.m_Taps;
        m_State = other.m_State;
    }

    //! Allow moving for building polyphase structures.
    poly_subfilter(poly_subfilter &&other)
    {
        if (this == &other)
            return;

        m_Taps = std::move(other.m_Taps);
        m_State = std::move(other.m_State);
    }

    //! Allow moving for building polyphase structures.
    poly_subfilter& operator=(poly_subfilter &&other)
    {
        if (this == &other)
            return *this;

        m_Taps = std::move(other.m_Taps);
        m_State = std::move(other.m_State);

        return *this;
    }

private:

    util::aligned_ptr<float> m_Taps;
    util::aligned_ptr<T> m_State;
};

}
