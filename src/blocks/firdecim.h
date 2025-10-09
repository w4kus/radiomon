// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <memory>
#include <complex>
#include <type_traits>
#include <aligned_ptr.h>
#include <vector>

#include "firfilt.h"
#include "block.h"

namespace dsp {

/*! \brief Decimation using a FIR filter.
 *
 * This implements decimation using a FIR filter, supplied by
 * the user, and a sampling buffer. Only real taps are supported.
*/

template<typename T, typename B>
class firdecim : public block<B>
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert(is_block_func_v<B>);

public:

    //! Create an instance with a integer interpolation factor and FIR filter.
    //! @param [in] M       The integer decimation factor,
    //! @param [in] taps    The filter coefficients.
    firdecim(const uint16_t M, const util::aligned_ptr<float> taps) :
                m_M { M }, m_SamplingCount { 0 }
    {
        assert(M > 0);

        block<B>::process = std::bind(&firdecim::decim, this, std::placeholders::_1, std::placeholders::_2);
        m_LpFilter = std::make_unique<firfilter<T, B>>(taps);
    }

    //! Decimate a block of a signal.
    //! @param [in]     inBlock     The data to be decimated.
    //! @param [out]    outBlock    The decimated data which will be the size of *inBlock* / **M** or
    //!                             the size of *inBlock* / **M** + 1.
    void decim(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        // If the sampling buffer is in an invalid state, validate it with the size of the
        // current block + the decimation factor.
        // Otherwise, check if it can hold the current block plus the current contents and,
        // if not, re-allocate a larger buffer and continue.
        if (m_SamplingBuffer.invalid())
            m_SamplingBuffer = util::make_aligned_ptr<T>(inBlock.size() + m_M);
        else if ((inBlock.size() + m_SamplingCount) > m_SamplingBuffer.size())
            m_SamplingBuffer = util::make_aligned_ptr<T>(inBlock.size() + m_SamplingCount + m_M, m_SamplingBuffer.get());

        // Create the filter buffer and filter the current block
        auto filterBlock = util::make_aligned_ptr<T>(inBlock.size());
        m_LpFilter->filter(inBlock, filterBlock);

        // Append the newly filtered data to the sampling buffer
        size_t mod = m_SamplingCount % m_M;
        std::copy(filterBlock.begin(), filterBlock.end(), m_SamplingBuffer.begin() + mod);
        m_SamplingCount += inBlock.size();

        // Create the output buffer
        outBlock = util::make_aligned_ptr<T>(m_SamplingCount / m_M);

        // Sample at every M samples
        size_t cnt = 0;
        for (auto it = outBlock.begin(), jt = m_SamplingBuffer.begin();
            it != outBlock.end();
            ++it, jt+=m_M)
        {
            *it = *jt;
            cnt += m_M;
        }

        // Shift the remaining content in the sampling buffer for the next block
        mod = m_SamplingCount % cnt;
        std::move(m_SamplingBuffer.begin() + cnt, m_SamplingBuffer.begin() + cnt + mod, m_SamplingBuffer.begin());
        m_SamplingCount -= cnt;
    }

private:

    uint16_t m_M;
    uint16_t m_SamplingCount;

    std::unique_ptr<firfilter<T, B>> m_LpFilter;
    util::aligned_ptr<T> m_SamplingBuffer;
};

using firdecim_ff = firdecim<float,dsp::func_ff>;
using firdecim_cc = firdecim<std::complex<float>,dsp::func_cc>;

}
