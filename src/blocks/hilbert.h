// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <array>
#include <vector>
#include <memory>

#include "block.h"
#include "delay.h"

namespace dsp {

constexpr size_t  HILBERT_TAP_NUM = 401;
extern const std::array<float, HILBERT_TAP_NUM> hilbert_fir;

/*! \brief Hilbert Transformer
 *
 * The Hilbert transform generates an analytic complex signal from a real
 * signal. This block approximates it with a type 3 FIR filter (even order,
 * 90 degree phase shift, integer group delay). The analytic
 * signal expands what demodulation methods can be used at the expense
 * of an extra filtering step.
 *
 * This is designed primarily for audio frequency signals, e.g., AFSK, DPSK.
 * You can use the default coefficient array or supply your own coefficients
 * provided it's a type 3. An Octave script, *hilbert_fir.m*, is provided
 * to generate it.
 *
 * \tparam numTaps  The number of taps in the FIR filter. Defaults to the
 *                  internal FIR filter (HILBERT_TAP_NUM; see the source file).
 *
 * \note The type 3 Hilbert FIR has zeros at DC and Nyquist so there's
 * rolloff effects near those frequencies. As stated, the default filter
 * was designed for AFSK/DPSK audio signals and it works quite well at up
 * to 96K sampling rates:
 *
 * |Fs  | Transition | Passband         | 0.1dB cutoff | -3dB cutoff |
 * |:--:|:----------:|:----------------:|:------------:|:----------- |
 * |48K | 395Hz      | 204Hz - 23,796Hz | 188Hz        | 91Hz        |
 * |    |            |                  |              |             |
 * |96K | 790Hz      | 407Hz - 47,593Hz | 376Hz        | 182Hz       |
 *
 * The drawback is the number of taps required to be so generic. For a specific
 * application, especially at relatively low bandwidths, a smaller, targeted filter
 * might be more desirable.
 *
 * \note The type 3 Hibert FIR has zeroes in every other tap starting from
 * the center. A **half-Hilibert** FIR uses a different transfer function to
 * generate the taps plus it requires a low pass filter for the delay line. But
 * the number of multiplications when evaluating the Hilbert is cut in half.
 * This current implementation does **not** support the half-Hilbert; it
 * may be added in the future.
 */

template <size_t numTaps = HILBERT_TAP_NUM>
class hilbert : public block<dsp::func_fc>
{
    static_assert(numTaps & 1);

public:
    //! Create an instance of the Hilbert transform block.
    //! The default contructor uses the internal FIR filter.
    hilbert() : block<dsp::func_fc>{ TYPE_OPERATOR }
    {
        setup(hilbert_fir.data());
    }

    //! Create an instance of the Hilbert transform block for dynamically allocated taps (std::vector)
    //! @param [in] taps  The user supplied type III FIR filter to use.
    hilbert(const std::vector<float> &taps) : block<dsp::func_fc>{ TYPE_OPERATOR }
    {
        assert((taps.size() & 1) && (taps.size() == numTaps));
        setup(taps.data());
    }

    //! Create an instance of the Hilbert transform block for statically allocated taps (std::array)
    //! @param [in] taps  The user supplied type III FIR filter to use.
    hilbert(const std::array<float, numTaps> &taps) : block<dsp::func_fc>{ TYPE_OPERATOR }
    {
        setup(taps.data());
    }

    //! Apply the Hilbert transform to a block of real samples to produce a block of complex samples.
    //! @param [in]  inBlock     The block of real samples to transform.
    //! @param [out] outBlock    The block of complex samples which will be the same size as *inBlock*.
    void transform(const util::aligned_ptr<float> &inBlock, util::aligned_ptr<rm_math::complex_f> &outBlock)
    {
        util::init_aligned_ptr_on_resize<rm_math::complex_f>(outBlock, inBlock.size());

        for (size_t i = 0; i < inBlock.size(); i++)
        {
            float res = 0.0f;
            float jres = 0.0f;
            outBlock[i] = rm_math::complex_f{0.0f, 0.0f};

            // Update the state with the new sample.
            std::move_backward(m_State.begin(), m_State.end() - 1, m_State.end());
            m_State[0] = inBlock[i];

            // Generate the Q component.
            rm_math::dot_prod(&jres, m_State.data(), m_Taps.data(), m_State.size());

            // Generate the complex sample by taking the delayed input sample (I) and
            // the just calculated filtered sample (Q)
            res = m_Delay << inBlock[i];

            if (m_Delay.size() == m_GroupDelay)
                outBlock[i] = rm_math::complex_f{res, jres};
        }
    }

private:

    size_t m_GroupDelay;
    util::aligned_ptr<float> m_State;
    util::aligned_ptr<float> m_Taps;

    comps::delay<float, (numTaps - 1) / 2> m_Delay;

    void setup(const float taps[numTaps])
    {
        m_GroupDelay = (numTaps - 1) / 2;

        block<dsp::func_fc>::process = std::bind(&hilbert::transform, this, std::placeholders::_1, std::placeholders::_2);

        // State buffer which is equal to the number of taps.
        util::init_aligned_ptr(m_State, numTaps);
        std::fill(m_State.begin(), m_State.end(), 0);

        util::init_aligned_ptr(m_Taps, numTaps, taps);
    }
};

using hilbert_default = hilbert<>;
}
