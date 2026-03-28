// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <vector>
#include <array>
#include <memory>

#include "block.h"
#include "aligned-ptr.h"
#include "poly-subfilter.h"
#include "polyphase.h"
#include "trace.h"

namespace dsp {

/*! \file rational-resampler.h
 *
 * \brief Polyphase Rational Resampler
 *
 * Given a rational resampling factor **L/M**, where **L** is the interpolation factor and
 * **M** is the decimation factor, this block will resample a block of samples from the
 * current sampling rate Fs to a new sampling rate **Fs*M/L**.
 *
 * To find the values of **L** and **M**, find the least common multiple betweenn the current
 * sampling rate and the new sampling rate. The *lcm* function in Octave or Matlab can do
 * this. If **Fc** is the current sampling rate and **Fn** is the new sampling rate then
 * **L = dcm / Fc** and **M = dcm / Fn**.
 *
 * Next you need to develop a anti-imaging and anti-aliasing FIR filter with a cutoff
 * frequency equal to the Nyquist of the interpolated sampling rate **Fs * L** if you plan
 * to pass all acceptable frequencies. This block uses a polyphase structure so the FIR
 * coefficients must be *decomoposed* into a matrix with the number of rows equal to the
 * interpolation factor **L**. The Octave directory contains a script, *poly_decompose.m*,
 * which will do this. It will generate a C matrix but you will need to decide if you want
 * it dynamically allocated (std::vector) or statically allocated (std::array). See the file
 * *test-rational-resampler.cc* for examples on how to do this.
 *
 * If **L = 1** and **M > 1**, then this block reduces to decimation only and **M** is the number of
 * rows in the polyphase structure. Similarly, if **M > 1** and **L = 1**, then this reduces
 * to interpolation only and **L** equals the number of rows in the polyphase structure.
 *
 */

template<typename T, typename B>
class rational_resampler : public block<B>
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert(is_block_func_v<B>);

public:

    //! Create an instance from a std::vector (dynamic memory)
    //! @param [in] L       The interpolation factor.
    //! @param [in] M       The decimation factor.
    //! @param [in] taps    The FIR coeffcients in polyphase decomposed form.
    //! @param [in] gain    The gain which is multipled to each coefficient.
    rational_resampler(const uint16_t L, const uint16_t M,
                        const std::vector<std::vector<float>> &taps, const uint16_t gain = 1) :
                        m_L { L }, m_M { M }, m_Mk { 0 }, m_DecimSum { 0 }
    {
        assert((L == taps.size()) || (M == taps.size()));

        bindCallbacks();
        m_SubFilters = util::polyBuildFilter<T>(taps, gain);
    }

    //! Create an instance from a std::array (static memory)
    //! @param [in] L       The interpolation factor.
    //! @param [in] M       The decimation factor.
    //! @param [in] taps    The FIR coeffcients in polyphase decomposed form.
    //! @param [in] gain    The gain which is multipled to each coefficient.
    template<size_t R, size_t C>
    rational_resampler(const uint16_t L, const uint16_t M,
                        const std::array<std::array<float, C>, R> &taps,  const uint16_t gain = 1) :
                        m_L { L }, m_M { M }, m_Mk { 0 }, m_DecimSum { 0 }
    {
        assert((L == R) || (M == R));

        bindCallbacks();
        m_SubFilters = util::polyBuildFilter<T, R, C>(taps, gain);
    }

    //! Resample a block of data
    //! @param [in]  inBlock    The block of input samples.
    //! @param [out] outBlock   The block of processed samples.
    void resample(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        m_Handler(inBlock, outBlock);
    }

private:

    using index_t = uint16_t;

    std::vector<comps::poly_subfilter<float>> m_SubFilters;

    uint16_t m_L;
    uint16_t m_M;
    uint16_t m_Mk;

    T m_DecimSum;

    std::function<B> m_Handler;
    std::unique_ptr<T[]> m_InterpDecimBuff;

    static constexpr char const *ID = "RR";
    util::trace<> m_Trace;

    //// helper
    void bindCallbacks()
    {
        // Interpolate?
        if ((m_L > 1) && (m_M == 1))
            m_Handler = std::bind(&rational_resampler::interp, this, std::placeholders::_1, std::placeholders::_2);

        // Decimate?
        else if ((m_L == 1) && (m_M > 1))
        {
            m_Mk = 1;
            m_Handler = std::bind(&rational_resampler::decim, this, std::placeholders::_1, std::placeholders::_2);
        }

        // Must be both - or both are one in which case the firfilt block is easier to use
        else
        {
            assert(!((block<B>::m_SamplingRate * m_L) % m_M));
            m_Mk = m_L;
            m_InterpDecimBuff = std::make_unique<T[]>(m_L);
            m_Handler = std::bind(&rational_resampler::interp_decim, this, std::placeholders::_1, std::placeholders::_2);
        }

        block<B>::process = std::bind(&rational_resampler::resample, this, std::placeholders::_1, std::placeholders::_2);
    }

    //// callbacks
    // Interpolation
    void interp(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        size_t sz = inBlock.size() * m_L;
        size_t k = 0;

        util::init_aligned_ptr_on_resize<T>(outBlock, sz);

        for (size_t i=0;i < sz;i++)
        {
            for (size_t j=0;j < m_L;j++)
                outBlock[k++] = m_SubFilters[j].insert(inBlock[i]);
        }
    }

    ////
    // Decimation
    void decim(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        size_t sz = inBlock.size() / m_M;
        size_t mod = inBlock.size() % m_M;
        size_t inIdx = 0;
        size_t outIdx = 0;

        util::init_aligned_ptr_on_resize<T>(outBlock, sz);

        while(inIdx < (inBlock.size() + mod))
        {
            --m_Mk;
            m_DecimSum += m_SubFilters[m_Mk].insert(inBlock[inIdx++]);

            if (!m_Mk)
            {
                outBlock[outIdx++] = m_DecimSum;
                m_Mk = m_M;
                m_DecimSum = 0;
            }
        }
    }

    ////
    // Interpolation -> decimation
    void interp_decim(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        size_t sz = inBlock.size() * m_L / m_M;
        size_t outIdx = 0;

        util::init_aligned_ptr_on_resize<T>(outBlock, sz);

        for (size_t i=0;i < inBlock.size();i++)
        {
            for (size_t j=0;j < m_L;j++)
                m_InterpDecimBuff[j] = m_SubFilters[j].insert(inBlock[i]);

            m_Mk -= m_L;

            while (m_Mk < m_L)
            {
                outBlock[outIdx++] = m_InterpDecimBuff[m_Mk];
                m_Mk += m_M;
            }
        }
    }
};

using rational_resampler_ff = rational_resampler<float,dsp::func_ff>;
using rational_resampler_cc = rational_resampler<std::complex<float>,dsp::func_cc>;

}
