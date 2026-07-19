// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include "block.h"
#include "zmq/sample-msg.h"

namespace dsp {

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
  * \tparam size         The size of a std::array if one is used for the header. Default is zero in which
  *                      case a string is used for the header.
*/

template<typename T, typename B, block_type type, uint8_t size = 0>
class zmq_sample_pub : public block<B>
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert((type == TYPE_OPERATOR) || (type == TYPE_SINK));

public:

    //! Create an instance using a C string as the ZMQ header
    //! @param [in] socketId  The socket ID to use. The subscriber must use the same ID.
    //! @param [in] hdr       A C string representing the header. The subscriber can use this for filtering.
    zmq_sample_pub(const char *socketId, const char *hdr) : block<B> { type }, m_Msg { hdr }
    {
        m_Msg.init(socketId);
        block<B>::process = std::bind(&zmq_sample_pub::handler, this, std::placeholders::_1, std::placeholders::_2);
    }

    //! Create an instance using a byte array as the ZMQ header.
    //! @param [in] socketId  The socket ID to use. The subscriber must use the same ID.
    //! @param [in] hdr       A std::array representing the header. The subscriber can use this for filtering. The
    //!                       size of the array is limited to 256 bytes.
    zmq_sample_pub(const char *socketId, const std::array<uint8_t, size> &hdr) : block<B> { type }, m_Msg { hdr }
    {
        static_assert(size > 0);

        m_Msg.init(socketId);
        block<B>::process = std::bind(&zmq_sample_pub::handler, this, std::placeholders::_1, std::placeholders::_2);
    }

    //! Handle incoming samples.
    //! @param [in]  inBlock    The block of input samples.
    //! @param [out] outBlock   The block of processed samples. This only applies if the block type is TYPE_OPERATOR.
    //!                         Otherwise, it's ignored.
    void handler(const util::aligned_ptr<T> &in, util::aligned_ptr<T> &out)
    {
        // Send to the socket
        m_Msg.send(in);

        // Copy in -> out
        if (block<B>::getType() == TYPE_OPERATOR)
        {
            util::init_aligned_ptr_on_resize(out, in.size());
            std::copy(in.begin(), in.end(), out.begin());
        }
    }

private:
    util::zmq::sample_msg<T, util::zmq::PUB_EP, size> m_Msg;
};

// Convenient aliases for float->float operator and sink objects using string headers
template<block_type T>
using zmq_sample_pub_ff = zmq_sample_pub<float, func_ff, T>;
using zmq_sample_pub_ff_op = zmq_sample_pub_ff<TYPE_OPERATOR>;
using zmq_sample_pub_ff_snk = zmq_sample_pub_ff<TYPE_SINK>;

// Convenient aliases for complex->complex operator and sink objects using string headers
template<block_type T>
using zmq_sample_pub_cc = zmq_sample_pub<std::complex<float>, func_cc, T>;
using zmq_sample_pub_cc_op = zmq_sample_pub_cc<TYPE_OPERATOR>;
using zmq_sample_pub_cc_snk = zmq_sample_pub_cc<TYPE_SINK>;

// Convenient aliases for float->float operator and sink objects using byte array headers
template<block_type T, uint8_t size>
using zmq_sample_pub_ff_array = zmq_sample_pub<float, func_ff, T, size>;

template<uint8_t size>
using zmq_sample_pub_ff_op_array = zmq_sample_pub<float, func_ff, TYPE_OPERATOR, size>;

template<uint8_t size>
using zmq_sample_pub_ff_snk_array = zmq_sample_pub<float, func_ff, TYPE_SINK, size>;

// Convenient aliases for complex->complex operator and sink objects using byte array headers
template<block_type T, uint8_t size>
using zmq_sample_pub_cc_array = zmq_sample_pub<std::complex<float>, func_cc, T, size>;

template<uint8_t size>
using zmq_sample_pub_cc_op_array = zmq_sample_pub<std::complex<float>, func_cc, TYPE_OPERATOR, size>;

template<uint8_t size>
using zmq_sample_pub_cc_snk_array = zmq_sample_pub<std::complex<float>, func_cc, TYPE_SINK, size>;

}
