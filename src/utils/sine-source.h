// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <type_traits>
#include <rm-math.h>

#include "aligned-ptr.h"

namespace util {

/*! \brief Sinusoidal Signal Generator Common Routines
 *
 */

template<typename T>
class sine_source_base
{
protected:
    sine_source_base() { }

    T m_Freq;
    T m_Phase;
    T m_Gain;

public:
    //! Return the current frequency in radians / sample.
    T frequency() { return m_Freq; }

    //! Set a new frequency.
    //! @param [in] freq The frequency in radians / sample.
    void setFrequency(T freq)
    {
        m_Freq = freq;
    }

    //! Return the current gain.
    T gain() { return m_Gain; }

    //! Set a new gain.
    //! @param [in] gain The new gain.
    void setGain(T gain)
    {
        m_Freq = gain;
    }
};

/*! \brief Sinusoidal Signal Generator for Floating Point Types
 *
 */

template<typename T>
class sine_source : public sine_source_base<T>
{
    static_assert(std::is_floating_point<T>::value == std::true_type());
    using base = sine_source_base<T>;

public:
    sine_source() = delete;

    //! Create a new instance.
    //! @param [in] freq    The initial frequency in radians / sample.
    //! @param [in] phase   The initial phase in radians. Default is zero.
    //! @param [in] gain    The **linear** gain. Default is one.
    sine_source(T freq, T phase = 0.0f, T gain = 1.0f)
    {
        base::m_Freq = freq;
        base::m_Phase = phase;
        base::m_Gain = gain;
    }

    //! Calculate and return a set of samples
    //! @param [inout] outBlock   An initialzed *aligned_ptr* with the required size which
    //!                           will be returned with the samples.
    void get(aligned_ptr<T> &outBlock)
    {
        for (size_t i=0;i < outBlock.size();i++)
        {
            outBlock[i] = base::m_Gain * rm_math::cos(base::m_Phase);

            base::m_Phase += base::m_Freq;
            if (base::m_Phase > (2 * M_PI))
                base::m_Phase -= 2 * M_PI;
        }
    }
};

/*! \brief Sinusoidal Signal Generator for Complex Types
 *
 */
template<>
class sine_source<complex_f> : public sine_source_base<float>
{
public:
    sine_source() = delete;

    //! Create a new instance.
    //! @param [in] freq    The initial frequency in radians / sample.
    //! @param [in] phase   The initial phase in radians.
    //! @param [in] gain    The **linear** gain.
    sine_source(float freq, float phase = 0.0f, float gain = 1.0f)
    {
        m_Freq = freq;
        m_Phase = phase;
        m_Gain = gain;
    }

    //! Calculate and return a set of samples
    //! @param [inout] outBlock   An initialzed *aligned_ptr* with the required size which
    //!                           will be returned with the samples.
    void get(aligned_ptr<complex_f> &out)
    {
        for (size_t i=0;i < out.size();i++)
        {
            out[i].real(m_Gain * rm_math::cos(m_Phase));
            out[i].imag(m_Gain * rm_math::sin(m_Phase));

            m_Phase += m_Freq;
            if (m_Phase > (2 * M_PI))
                m_Phase -= 2 * M_PI;
        }
    }
};

}
