// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <type_traits>

#include "block.h"
#include "sine-source.h"

namespace dsp { namespace endpoints {

/*! \brief Signal Source Endpoint
 *
 * This endpoint source generates a sine wave given a specified frequency and sampling rate.
 *
 * \note The sampling rate is used for signal generation calculations only and this block
 * does not attempt to produce samples at the specified rate.
*/

template<typename T, typename B>
class signal_source : public block<B>
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert(is_block_func_v<B>);

public:

    //! Create a new instance.
    //! @param [in] size         The number of samples to generate per block.
    //! @param [in] freq         The signal frequency in Hz.
    //! @param [in] samplingRate The sampling rate in Hz.
    //! @param [in] phase        The initial phase in radians.
    //! @param [in] gain         The **linear** gain.
    signal_source(const size_t size, const rate_t freq, const rate_t samplingRate,
                    const float phase = 0.0f, const float gain = 1.0f) :
                        block<B> { TYPE_SOURCE },
                        m_Sine { rm_math::hz_to_rps(freq, samplingRate), phase, gain },
                        m_Size { size }, m_SamplingRate { samplingRate }
    {
        block<B>::process = std::bind(&signal_source::generate, this, std::placeholders::_1, std::placeholders::_2);
    }

    //! Generate a block of samples.
    //! @param [in]  inBlock    Ignored. This will be removed in a future commit.
    //! @param [out] outBlock   The block of generated samples.
    void generate(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        // Endpoint sources must set the sampling rate on each block processing call
        block<B>::m_SamplingRate = m_SamplingRate;

        util::init_aligned_ptr_on_resize<T>(outBlock, m_Size);
        m_Sine.get(outBlock);
    }

private:

    util::sine_source<T> m_Sine;
    size_t m_Size;
    rate_t m_SamplingRate;
};

using signal_source_ff = signal_source<float, dsp::func_ff>;
using signal_source_cc = signal_source<rm_math::complex_f, dsp::func_cc>;

}}
