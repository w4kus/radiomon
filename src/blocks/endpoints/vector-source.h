// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <type_traits>
#include <vector>
#include <array>
#include <functional>

#include "block.h"

namespace dsp { namespace endpoints {

/*! \brief Vector Source Endpoint
 *
 * This block serves as a source of samples for the signal processing chain.
 * It outputs the contents of a vector of samples, which is specified at construction time.
 *
*/

template<typename T, typename B, size_t R = 0>
class vector_source : public block<B>
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert(is_block_func_v<B>);

public:

    //! Create a new instance.
    //! @param [in] data    The vector of samples to output.
    vector_source(const std::vector<T> &data, const rate_t rate) : block<B> { TYPE_SOURCE },
                    m_Data { data }, m_SamplingRate { rate }
    {
        block<B>::process = std::bind(&vector_source::generate, this, std::placeholders::_1, std::placeholders::_2);
        m_Callback = std::bind(&vector_source::vectorHandler, this, std::placeholders::_1, std::placeholders::_2);
    }

    //! Create a new instance.
    //! @param [in] data    The vector of samples to output.
    vector_source(const std::array<T, R> &data, const rate_t rate) : block<B> { TYPE_SOURCE }, m_SamplingRate { rate }
    {
        static_assert(R);

        m_StaticData = &data;
        block<B>::process = std::bind(&vector_source::generate, this, std::placeholders::_1, std::placeholders::_2);
        m_Callback = std::bind(&vector_source::arrayHandler, this, std::placeholders::_1, std::placeholders::_2);
    }

    //! Generate a block of samples.
    //! @param [in]  inBlock    Ignored. This will be removed in a future commit.
    //! @param [out] outBlock   The block of generated samples.
    void generate(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        // Endpoint sources must set the sampling rate on each block processing call
        block<B>::m_SamplingRate = m_SamplingRate;
        m_Callback(inBlock, outBlock);
    }

private:
    std::vector<T>          m_Data;
    const std::array<T, R>  *m_StaticData;

    std::function<B> m_Callback;

    rate_t m_SamplingRate;

    void vectorHandler(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        util::init_aligned_ptr_on_resize<T>(outBlock, m_Data.size());
        std::copy(m_Data.begin(), m_Data.end(), outBlock.begin());
    }

    void arrayHandler(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        util::init_aligned_ptr_on_resize<T>(outBlock, m_StaticData->size());
        std::copy(m_StaticData->begin(), m_StaticData->end(), outBlock.begin());
    }
};

using vector_source_ff = vector_source<float, dsp::func_ff>;
using vector_source_cc = vector_source<rm_math::complex_f, dsp::func_cc>;

}}
