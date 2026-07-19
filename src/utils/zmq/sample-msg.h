// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <zmq.h>
#include <complex>
#include <string>
#include <array>

#include "context.h"
#include "rm-math.h"

namespace util { namespace zmq {

/*! \brief ZMQ Sample Bridge
 *
 * Uses the ZMQ sockets library to send or receive samples from an external process. The interface
 * is designed to be used by blocks but apps can use it as well.
 *
 * \tparam T            The type of *aligned_ptr* to create. Either *float* or *std::vector<float>*.
 * \tparam sock_type    The socket type to use. See *socket_ep_t* in context.h.
 * \tparam array_size   The size of the header array if used. Defaults to zero which means a string is
 *                      used for the header.
 *
 * \note This sets up an IPC (unix) socket which is currently not available on Windows. See the ZMQ
 * documentation for details. Adding TCP/UDP/etc.. should be trivial but it has not been tried and
 * tested yet.
 */

template<typename T, socket_ep_t sock_type, uint8_t array_size = 0>
class sample_msg : public context
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert((sock_type == PUB_EP) || (sock_type == SUB_EP));

public:

    //! Create an instance using a std::string containing the header to send.
    sample_msg(const std::string &hdr) : m_sMsgHdr { hdr }, m_Sock { nullptr }
    {
        m_MsgHdr = m_sMsgHdr.data();
        m_MsgHdrLen = hdr.size();
    }

    //! Create an instance using a std::array of bytes containing the header to send.
    sample_msg(const std::array<uint8_t, array_size> &hdr) : m_aMsgHdr { hdr }, m_Sock { nullptr }
    {
        static_assert(array_size > 0);

        m_MsgHdr = m_aMsgHdr.data();
        m_MsgHdrLen = hdr.size();
    }

    ~sample_msg()
    {
        if (m_Sock) zmq_close(m_Sock);
    }

    sample_msg() = delete;

    //! Initialize and setup the socket.
    //! @param [in] socketId  A string containing an id for the socket. The sender (publisher) and
    //! receiver (subscriber) must use the same ID. It can be any printable character and is
    //! limited to 64 characters.
    //! @return *true* if successful, *false* otherwise. *errno* will contain the error code.
    bool init(const char *socketId)
    {
        if (!m_Sock)
        {
            char endpoint[65];
            uint8_t epStrSize = strlen(socketId) + strlen(ep_str);

            assert(epStrSize < sizeof(endpoint));

#ifdef NDEBUG
            if (epStrSize >= sizeof(endpoint))
            {
                m_Trace.print(ID, "EP string overfloat\n");
                return false;
            }
#endif
            memset(endpoint, 0, sizeof(endpoint));
            snprintf(endpoint, sizeof(endpoint) - 1, "%s%s", ep_str, socketId);

            m_Sock = zmq_socket(get(), sock_type);
            assert(m_Sock);

#ifdef NDEBUG
            if (!m_Sock)
            {
                m_Trace.print(ID, "Socket creation failed: %d\n", errno);
                return false;
            }
#endif
            int val = 0;
            // Don't linger when the term signal is sent
            zmq_setsockopt(m_Sock, ZMQ_LINGER, &val, sizeof(int));

            if (sock_type == PUB_EP)
            {
                val = zmq_bind(m_Sock, endpoint);
                checkInt(val, "zmq_bind");
            }
            else
            {
                val = zmq_connect(m_Sock, endpoint);
                checkInt(val, "zmq_connect");

                zmq_setsockopt(m_Sock, ZMQ_SUBSCRIBE, m_MsgHdr, m_MsgHdrLen);
            }

#ifdef NDEBUG
            if (val == -1)
            {
                m_Trace.print(ID, "zmq_bind/zmq_connect failure: %d\n", errno);
                return false;
            }
#endif
            m_Trace.print(ID, "%s ipc socket at %s\n", (sock_type == PUB_EP) ? "PUB" : "SUB", endpoint);
        }

        return true;
    }

    //! Send a block of samples to the subscriber.
    //! @param [in] samples  The samples to send
    //! @return Zero if successful, -1 otherwise. *errno* will contain the error code.
    int send(const aligned_ptr<T> &samples)
    {
        int rc = -1;
        errno = EFAULT;

        if (m_Sock)
        {
            zmq_msg_t msg;

            rc = zmq_msg_init_size(&msg, m_MsgHdrLen);
            checkMsg(rc, "zmq_msg_init_size", msg);

            memcpy(zmq_msg_data(&msg), m_MsgHdr, m_MsgHdrLen);

            rc = zmq_msg_send(&msg, m_Sock, ZMQ_SNDMORE);
            checkMsg(rc, "zmq_msg_send", msg);

            rc = zmq_msg_init_size(&msg, samples.size() * sizeof(T));
            checkMsg(rc, "zmq_msg_init_size", msg);

            memcpy(zmq_msg_data(&msg), samples.data(), samples.size() * sizeof(T));

            rc = zmq_msg_send(&msg, m_Sock, 0);
            checkMsg(rc,"zmq_msg_send", msg);
        }

        return rc;
    }

    //! Receive a block of samples from the publisher.
    //! @param [out] samples  The received block of samples if successful; undefined otherwise.
    //! @param [in]  block    If *true* this will block until samples are received.
    //! @return Zero if successful, -1 otherwise. *errno* will contian the error code. If
    //! operating in non-blocking mode and there's no samples available, this will return
    //! -1 and *errno* will be set to *EAGAIN*.
    int recv(aligned_ptr<T> &samples, bool block = false)
    {
        int rc = -1;
        errno = EFAULT;

        if (m_Sock)
        {
            zmq_msg_t msg;
            int partIdx = 0;

            while(1)
            {
                // always returns success
                zmq_msg_init(&msg);

                rc = zmq_msg_recv(&msg, m_Sock, (!block) ? ZMQ_DONTWAIT : 0);

                // The doc says that all messages, multi-part or not, are delivered in its
                // entirety, so once we start receiving the parts (if it is multi-part), we
                // should receive all the parts.
                if (rc == -1)
                {
                    if (errno != EAGAIN)
                    {
                        m_Trace.print(ID, "Failure on receive: %d\n", errno);
                        assert(1);
                        return -1;
                    }

                    zmq_msg_close(&msg);
                    return rc;
                }

                if (!partIdx)
                {
                    bool hdrMatch = false;

                    // Check the header
                    if ((array_size > 0) && (m_aMsgHdr.size() == static_cast<size_t>(rc)))
                    {
                        uint8_t *hdr = static_cast<uint8_t *>(zmq_msg_data(&msg));
                        hdrMatch = !memcmp(hdr, m_aMsgHdr.data(), rc);
                    }
                    else if (m_sMsgHdr.size() == static_cast<size_t>(rc))
                    {
                        char *hdr = static_cast<char *>(zmq_msg_data(&msg));
                        hdrMatch = !memcmp(hdr, m_sMsgHdr.data(), rc);
                    }

                    if (!hdrMatch)
                    {
                        rc = -1;
                        errno = EAGAIN;
                        break;
                    }

                    ++partIdx;
                }
                else
                {
                    init_aligned_ptr_on_resize<T>(samples, rc / sizeof(T));
                    memcpy(&samples[0], zmq_msg_data(&msg), rc);
                    ++partIdx;
                    rc = 0;
                }

                zmq_msg_close(&msg);

                if (partIdx == 2)
                    break;
            }

            // clear out remaing parts if any
            while(zmq_msg_more(&msg))
                zmq_msg_close(&msg);
        }

        return rc;
    }

private:

    std::string                       m_sMsgHdr;
    std::array<uint8_t, array_size>   m_aMsgHdr;
    int                               m_MsgHdrLen;
    msg_hdr_t                         m_MsgHdr;
    sock_t                            m_Sock;

    static constexpr char const *ep_str = "ipc:///tmp/radiomon_sock-";
};

}}
