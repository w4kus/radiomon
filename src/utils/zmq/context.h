// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <zmq.h>
#include <complex>

#include "aligned-ptr.h"
#include "trace.h"

namespace util { namespace zmq {

using ctx_t         =  void*;
using sock_t        =  void*;
using msg_hdr_t     =  const void*;

enum socket_ep_t
{
    PAIR_EP     = ZMQ_PAIR,
    PUB_EP      = ZMQ_PUB,
    SUB_EP      = ZMQ_SUB,
    REQ_EP      = ZMQ_REQ,
    REP_EP      = ZMQ_REP,
    DEALER_EP   = ZMQ_DEALER,
    ROUTER_EP   = ZMQ_ROUTER,
    PULL_EP     = ZMQ_PULL,
    PUSH_EP     = ZMQ_PUSH,
    XPUB_EP     = ZMQ_XPUB,
    XSUB_EP     = ZMQ_XSUB,
    STREAM_EP   = ZMQ_STREAM
};

/*! \brief ZMQ Context Wrapper with Helpers
 *
 * Manages the global ZMQ context. To use, it's best to create and derive a
 * class from this object since it does reference counting. Once all references
 * have been destroyed, it frees the context. Multiple ZMQ subclasses in an app
 * can be created with the only limitations being RAM and the number of allowed
 * threads (the default is 1024 which is currently used).
 *
 */

class context
{
public:

    //! Create an instance which increases the reference count. Due to reference
    //! couting, you cannot copy instances but you can move them.
    context();
    ~context();

    context(const context &) = delete;
    context& operator=(const context&) = delete;

    //! Get a pointer to the global context which allows the caller to use
    //! the ZMQ C API directly.
    //! @note It is preferable to create an object derived off this class to implement
    //! your requirements. See *sample-msg.h* for an example.
    //! @return A pointer to the context.
    const ctx_t get() const;

protected:

    //! \cond

    static constexpr char const *ID = "ZMQ";
    util::trace<true> m_Trace;

    void checkMsg(const int rc, const char *prompt, zmq_msg_t &msg);
    void checkPtr(const void *p, const char *prompt);
    void checkInt(const int rc, const char *prompt);

    //! \endcond
};

}}

