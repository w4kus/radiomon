// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <zmq.h>
#include <complex>
#include <string>
#include <array>
#include <type_traits>

#include <stdio.h>

#include "context.h"
#include "rm-math.h"

namespace util { namespace zmq {

/*! \brief General Sample Streaming via ZMQ PUB/SUB sockets
 *
 * Uses the ZMQ sockets library to send or receive samples from an external process. The interface
 * is designed to be used by blocks but apps can use it as well. Typically you use TCP sockets for
 * the transport.
 *
 * \tparam T  The sample type; either *float* or *rm_math::complex_f*.
 * \tparam sock_type  Either PUB_EP (publisher) or SUB_EP (subscriber).
 */

template<typename T, socket_ep_t sock_type>
class sample_stream : public context
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert((sock_type == PUB_EP) || (sock_type == SUB_EP));

public:

    //! Create an instance
    sample_stream() : m_ElemScale { 1.0f / sizeof(T) }
    {
        m_Sock = zmq_socket(get(), sock_type);
        assert(m_Sock);

#ifdef NDEBUG
        if (!m_Sock)
        {
            m_Trace.print(ID, "Socket creation failed: %d\n", errno);
            std::abort();
        }
#endif
    }

    //! Set the URL to use when setting up the socket
    //! @param [in] url  A default is used if not specified. The format is
    //!                  "<protocol>//<host>:<port>". The recommended
    //!                  protocol is *tcp*. The default is "tcp://127.0.0.1:55555".
    void setURL(const char *url = DEFAULT_URL)
    {
        if (m_Url.empty())
        {
            int val = 0;

            // Don't linger when the term signal is sent
            zmq_setsockopt(m_Sock, ZMQ_LINGER, &val, sizeof(int));

            m_Url = url;

            printf("%s\n", m_Url.c_str());

            if (sock_type == PUB_EP)
            {
                val = zmq_bind(m_Sock, m_Url.c_str());
                checkInt(val, "zmq_bind");
            }
            else
            {
                val = zmq_connect(m_Sock, m_Url.c_str());
                checkInt(val, "zmq_connect");

                zmq_setsockopt(m_Sock, ZMQ_SUBSCRIBE, nullptr, 0);
            }
        }
    }

    //! Send a block of samples if configured as a publisher.
    //! @param [in] samples  The block of samples to send.
    //! @return The number of bytes sent, zero if it couldn't be queued, or -1 if an error.
    //!         Check *errno* for the error code.
    int send(const aligned_ptr<T> &samples)
    {
        assert(!m_Url.empty());

        errno = EFAULT;
        int rc = zmq_send(m_Sock, samples.data(), samples.size() * sizeof(T), ZMQ_DONTWAIT);

        if ((rc == -1) && (errno == EAGAIN))
            rc = 0;

        return static_cast<int>(rc * m_ElemScale);
    }

    //! Receive a block of samples if configured as a subscriber.
    //! @param [out] samples  A buffer to receive the samples. This method will
    //!                       re-size the buffer if necessary.
    //! @param [in]  block    Whether or not to block if no samples are available yet.
    //! @return The number of bytes received, zero if it couldn't be queued, or -1 if an error.
    //!         Check *errno* for the error code.
    int recv(aligned_ptr<T> &samples, bool block = false)
    {
        assert(!m_Url.empty());

        errno = EFAULT;
        zmq_msg_t msg;
        zmq_msg_init(&msg);

        int rc = zmq_msg_recv(&msg, m_Sock, (block == false) ? ZMQ_DONTWAIT : 0);

        if ((rc == -1) && (errno == EAGAIN))
            rc = 0;
        else
        {
            auto sz = zmq_msg_size(&msg);
            util::init_aligned_ptr_on_resize(samples, sz * m_ElemScale);
            std::memcpy(&samples[0], zmq_msg_data(&msg), sz);
        }

        zmq_msg_close(&msg);

        return static_cast<int>(rc * m_ElemScale);
    }

private:
    sock_t      m_Sock;
    std::string m_Url;
    float       m_ElemScale;

    static constexpr char const *DEFAULT_URL = "tcp://127.0.0.1:55555";
};
}}
