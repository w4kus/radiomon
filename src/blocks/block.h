// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <functional>
#include <type_traits>

#include "aligned-ptr.h"
#include "rm-math.h"

namespace dsp {

//! \cond
//////////////////////////
template<typename>
struct is_block_func : public std::false_type {};

template<>
struct is_block_func<void(const util::aligned_ptr<float>&, util::aligned_ptr<float>&)> :
        public std::true_type {};

template<>
struct is_block_func<void(const util::aligned_ptr<float>&, util::aligned_ptr<rm_math::complex_f>&)> :
        public std::true_type {};

template<>
struct is_block_func<void(const util::aligned_ptr<rm_math::complex_f>&, util::aligned_ptr<rm_math::complex_f>&)> :
        public std::true_type {};

template<>
struct is_block_func<void(const util::aligned_ptr<rm_math::complex_f>&, util::aligned_ptr<float>&)> :
        public std::true_type {};

template<typename T>
constexpr bool is_block_func_v = is_block_func<T>::value;

//! \endcond
//////////////////////////

using func_ff = void(const util::aligned_ptr<float>&, util::aligned_ptr<float>&);
using func_fc = void(const util::aligned_ptr<float>&, util::aligned_ptr<rm_math::complex_f>&);
using func_cc = void(const util::aligned_ptr<rm_math::complex_f>&, util::aligned_ptr<rm_math::complex_f>&);
using func_cf = void(const util::aligned_ptr<rm_math::complex_f>&, util::aligned_ptr<float>&);

using rate_t = uint32_t;

enum block_type
{
    TYPE_OPERATOR,  // A block which performs some processing on the input samples and produces output samples.
    TYPE_SOURCE,    // A block which generates input samples for the signal processing chain.
    TYPE_SINK,      // A block which consumes output samples from the signal processing chain.
    TYPE_BIDIR,     // A block which serves as both a source and sink in the signal processing chain.
    TYPE_RESAMPLER  // An operator type block which resamples the signal.
};

enum bidir_mode
{
    BIDIR_NONE,
    BIDIR_SOURCE,
    BIDIR_SINK
};

/*! \brief Base Template for All DSP Blocks */

template<typename T>
class block
{
public:

    //! Get the block's processing method.
    auto        getProcesser() const { return process; }

    //! Set the current sampling rate for the block to use.
    void        setSamplingRate(uint32_t rate) { m_SamplingRate = rate; }

    //! Get the type of block this represents.
    block_type  getType() const { return m_Type; }

    //! Get the block's sampling rate. Resampling blocks change the sampling rate.
    rate_t      getSamplingRate() const { return m_SamplingRate; }

    //! Set the mode for a bidirectional block.
    void        setBidirMode(bidir_mode mode) { m_BidirMode = mode; }

protected:
    //! \cond

    block(block_type type) : m_SamplingRate { 0 },  m_BidirMode { BIDIR_NONE }, m_Type { type }
    { }

    block() = delete;

    block(const block &) = delete;
    block& operator=(const block &) = delete;

    block(const block&&) = delete;
    block& operator=(block &&) = delete;

    std::function<T> process;

    rate_t m_SamplingRate;

    bidir_mode m_BidirMode;

private:
    block_type  m_Type;

    //! \endcond
};

}
