// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include "block.h"
#include "zmq/sample-msg.h"

namespace dsp {

union value_rate_t
{
    uint8_t bytes[4];
    rate_t val;
};

constexpr uint8_t SAMPLES_FLOAT = 0x01;
constexpr uint8_t SAMPLES_CMPLX = 0x02;

/*! \brief ZMQ Publisher for Samples in a Chain
 *
 * This is a chain wrapper for ZMQ support. It simply forwards incoming samples to the
 * global ZMQ context. It can function as an operator or a sink with the only difference
 * being that an operator will copy the samples to the output buffer in addition to forwarding
 * them to the ZMQ context.
 *
  * \tparam T            The type of *aligned_ptr* to create. Either *float* or *std::vector<float>*.
  * \tparam B            The block function type. See block.h.
  * \tparam block_type   The block type. See block.h.
*/

template<typename T, typename B, block_type type>
class zmq_sample_pub : public block<B>
{
    static_assert(std::is_floating_point<T>::value == std::true_type());
    static_assert((type == TYPE_OPERATOR) || (type == TYPE_SINK));

public:

    //! Create an instance.
    //! @param [in] socketId  The socket ID to use.
    zmq_sample_pub(const char *socketId) : block<B> { type }
    {
        m_Hdr = def_hdr;
        m_Msg.header(m_Hdr);
        m_Msg.init(socketId);

        block<B>::process = std::bind(&zmq_sample_pub::handler, this, std::placeholders::_1, std::placeholders::_2);
    }

    //! Handle incoming samples.
    //! @param [in]  inBlock    The block of input samples.
    //! @param [out] outBlock   The block of processed samples. This only applies if the block type is TYPE_OPERATOR.
    //!                         Otherwise, it's ignored.
    void handler(const util::aligned_ptr<T> &in, util::aligned_ptr<T> &out)
    {
        value_rate_t rate;
        rate.val = block<B>::getSamplingRate();

        // Send to the socket
        m_Hdr[1] = rate.bytes[0];
        m_Hdr[2] = rate.bytes[1];
        m_Hdr[3] = rate.bytes[2];
        m_Hdr[4] = rate.bytes[3];

        m_Msg.header(m_Hdr);
        m_Msg.send(in);

        // Copy in -> out
        if (block<B>::getType() == TYPE_OPERATOR)
        {
            util::init_aligned_ptr_on_resize(out, in.size());
            std::copy(in.begin(), in.end(), out.begin());
        }
    }

private:
    static constexpr std::array<uint8_t, 5> def_hdr   = { SAMPLES_FLOAT, 0x00, 0x00, 0x00, 0x00 };

    util::zmq::sample_msg<T, util::zmq::PUB_EP, def_hdr.size()> m_Msg;
    std::array<uint8_t, def_hdr.size()> m_Hdr;
};

//! \cond

// Partial specialization for std::complex which requires a different header.
template<block_type type>
class zmq_sample_pub<rm_math::complex_f, func_cc, type> : public block<func_cc>
{
    static_assert((type == TYPE_OPERATOR) || (type == TYPE_SINK));

public:

    //! Create an instance.
    //! @param [in] socketId  The socket ID to use. The subscriber must use the same ID.
    zmq_sample_pub(const char *socketId) : block { type }
    {
        m_Hdr = def_hdr;
        m_Msg.header(m_Hdr);
        m_Msg.init(socketId);

        process = std::bind(&zmq_sample_pub::handler, this, std::placeholders::_1, std::placeholders::_2);
    }

    //! Handle incoming samples.
    //! @param [in]  inBlock    The block of input samples.
    //! @param [out] outBlock   The block of processed samples. This only applies if the block type is TYPE_OPERATOR.
    //!                         Otherwise, it's ignored.
    void handler(const util::aligned_ptr<rm_math::complex_f> &in, util::aligned_ptr<rm_math::complex_f> &out)
    {
        value_rate_t rate;
        rate.val = getSamplingRate();

        // Send to the socket
        m_Hdr[1] = rate.bytes[0];
        m_Hdr[2] = rate.bytes[1];
        m_Hdr[3] = rate.bytes[2];
        m_Hdr[4] = rate.bytes[3];

        m_Msg.header(m_Hdr);
        m_Msg.send(in);

        // Copy in -> out
        if (getType() == TYPE_OPERATOR)
        {
            util::init_aligned_ptr_on_resize(out, in.size());
            std::copy(in.begin(), in.end(), out.begin());
        }
    }

private:
    static constexpr std::array<uint8_t, 5> def_hdr   = { SAMPLES_CMPLX, 0x00, 0x00, 0x00, 0x00 };

    util::zmq::sample_msg<rm_math::complex_f, util::zmq::PUB_EP, def_hdr.size()> m_Msg;
    std::array<uint8_t, def_hdr.size()> m_Hdr;
};

//! \endcond


// Convenient aliases for float->float operator and sink objects
template<block_type T>
using zmq_sample_pub_ff = zmq_sample_pub<float, func_ff, T>;
using zmq_sample_pub_ff_op = zmq_sample_pub_ff<TYPE_OPERATOR>;
using zmq_sample_pub_ff_snk = zmq_sample_pub_ff<TYPE_SINK>;

// Convenient aliases for complex->complex operator and sink objects
template<block_type T>
using zmq_sample_pub_cc = zmq_sample_pub<std::complex<float>, func_cc, T>;
using zmq_sample_pub_cc_op = zmq_sample_pub_cc<TYPE_OPERATOR>;
using zmq_sample_pub_cc_snk = zmq_sample_pub_cc<TYPE_SINK>;

}
