// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <type_traits>
#include <string>

#include "block.h"
#include "zmq/sample-stream.h"

namespace dsp { namespace endpoints {

/*! \brief ZMQ Stream Source
 *
 * Provides a source for receiving samples from an external process. The motivation for this is to
 * utilize GNU Radio for testing but it can be used for any application that supports ZMQ sockets.
 * This sets up a SUBSCRIBER socket which is connected to the external applications PUBLISHER socket.
 * The TCP protocol is the preferred scheme and is required if using GNU Radio.
 *
 * \tparam T  The sample type, either *float* or *complex*.
 * \tparam B  The block processing function signature; use one of the aliases at the bottom.
 */

template<typename T, typename B>
class zmq_stream_source : public block<B>
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert(is_block_func_v<B>);

public:

    //! Create a new instance.
    //! @param [in] url    The URL to use; e.g., "tcp://127.0.0.1:55555" for a publisher on the local host.
    zmq_stream_source(const char *url) : block<B> { TYPE_SOURCE }
    {
        block<B>::process = std::bind(&zmq_stream_source::handleStream, this, std::placeholders::_1, std::placeholders::_2);
        m_ss.setURL(url);
    }

    //! Send a received block of samples.
    //! @param [in]  inBlock    Ignored.
    //! @param [out] outBlock   The block of generated samples.
    //! \note This routine blocks until samples arrive.
    void handleStream(const util::aligned_ptr<T> &in, util::aligned_ptr<T> &out)
    {
        // recv() resizes the buffer if necessary
        int rc = m_ss.recv(out, true);

        // Clear the buffer if an error occcurs
        if (rc == -1)
            out.clear();
    }

private:

    util::zmq::sample_stream<T, util::zmq::SUB_EP> m_ss;
};

using zmq_stream_source_ff = zmq_stream_source<float, dsp::func_ff>;
using zmq_stream_source_cc = zmq_stream_source<rm_math::complex_f, dsp::func_cc>;

}}
