// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <mutex>
#include <assert.h>
#include <memory.h>
#include "zmq/context.h"

using namespace util::zmq;

static ctx_t theContext = nullptr;
static std::mutex mtx;
static uint32_t refCount = 0;

context::context()
{
    std::lock_guard<std::mutex> lck(mtx);

    if (!theContext)
    {
        theContext = zmq_ctx_new();

        checkPtr(theContext, "zmq_ctx_new");

        // ZMQ_LINGER set to zero for all sockets.
        zmq_ctx_set(theContext, ZMQ_BLOCKY, 0);

        // Standard, non-real time round-robin thread scheduling ('nice' values).
        zmq_ctx_set(theContext, ZMQ_THREAD_SCHED_POLICY, SCHED_OTHER);

        // Numberic prefix for each of the context's threads in the thread pool (for debugging).
        zmq_ctx_set(theContext, ZMQ_THREAD_NAME_PREFIX, 338);
    }

    ++refCount;

    m_Trace.print(ID, "ref cnt %d\n", refCount);
}

context::~context()
{
    std::lock_guard<std::mutex> lck(mtx);

    --refCount;

    m_Trace.print(ID, "ref cnt %d\n", refCount);

    if (!refCount)
        zmq_ctx_term(theContext);
}

const ctx_t context::get() const
{
    return theContext;
}

void context::checkMsg(const int rc, const char *prompt, zmq_msg_t &msg)
{
#ifndef NDEBUG
    if (rc == -1)
    {
        m_Trace.print(ID, "%s:%d\n", prompt, errno);
        zmq_msg_close(&msg);
        assert(1);
    }
#endif
}

void context::checkPtr(const void  *p, const char *prompt)
{
#ifndef NDEBUG
    if (p == nullptr)
    {
        m_Trace.print(ID, "%s:%d\n", prompt, errno);
        assert(1);
    }
#endif
}

void context::checkInt(const int rc, const char *prompt)
{
#ifndef NDEBUG
    if (rc == -1)
    {
        m_Trace.print(ID, "%s:%d\n", prompt, errno);
        assert(1);
    }
#endif
}
