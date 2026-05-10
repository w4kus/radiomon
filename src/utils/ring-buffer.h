#pragma once

#include <mutex>
#include <thread>
#include <memory>
#include <functional>
#include <type_traits>
#include <atomic>
#include <cassert>

#include "timer.h"
#include "rm-math.h"

namespace util {

/*! \brief Ring Buffer
 *
 * Implements an SPSC ring buffer using a mod-2 buffer.
*/

template <typename T>
class ring_buffer
{
    static_assert((std::is_integral<T>::value == std::true_type()) ||
                    (std::is_floating_point<T>::value == std::true_type()) ||
                    (util::is_std_complex_v<T>));
public:

    //! Diagnostic structure returned in a call to *diagnostics()*.
    struct diag
    {
        //! The number of times the buffer was full and the caller had to wait.
        uint32_t    fullCount;
        //! The number of times the buffer contained less than the requested read amount;
        uint32_t    emptyCount;
    };

    //! Create an instance which uses dynamic memory for the buffer.
    //! @param [in] exp         Exponent of the base 2 radix which determines the buffer size.
    //! @param [in] yieldTime   Time in microseconds to yield while waiting until the amount meets the transfer criteria.
    ring_buffer(const uint8_t exp, const uint32_t yieldTime = 1000) :
                m_Capacity { (uint32_t)1 << exp },
                m_Mask { m_Capacity - 1 },
                m_YieldTime { yieldTime },
                m_StaticBuff { nullptr },
                m_WriteIdx { 0 },
                m_ReadIdx { 0 },
                m_Abort { 0 },
                m_WaitForNotFullCount { 0 },
                m_WaitForAmtCount { 0 }
    {
        assert(exp > 0);
        m_DynamicBuff = std::make_unique<T[]>(m_Capacity);
        m_ReadFunc = std::bind(&ring_buffer::dynamicRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        m_WriteFunc = std::bind(&ring_buffer::dynamicWrite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    //! Create an instance which uses a user supplied buffer. Primarily for statically declared buffers.
    //! @param [in] buff        Caller suplied buffer
    //! @param [in] size        The number of elements the buffer will hold where each element is sizeof(T).
    //! @param [in] yieldTime   Time in microseconds to yield while waiting until the amount meets the transfer criteria.
    //! \note  It is the responsibility of the caller to ensure the buffer size is mod-2.
    ring_buffer(const T *buff, const uint32_t size, const uint32_t yieldTime = 1000) :
                m_Capacity { size },
                m_Mask { m_Capacity - 1 },
                m_YieldTime { yieldTime },
                m_StaticBuff { (T *)buff },
                m_WriteIdx { 0 },
                m_ReadIdx { 0 },
                m_Abort { 0 },
                m_WaitForNotFullCount { 0 },
                m_WaitForAmtCount { 0 }
    {
        assert((size > 0) && !(size & (size - 1)));
        m_ReadFunc = std::bind(&ring_buffer::staticRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        m_WriteFunc = std::bind(&ring_buffer::staticWrite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    ring_buffer() = delete;

    ring_buffer(const ring_buffer&) = delete;
    ring_buffer& operator=(const ring_buffer&) = delete;

    ring_buffer(ring_buffer&&) = delete;
    ring_buffer& operator=(ring_buffer&&) = delete;

    //! Write data to the ring buffer. This will block until all data is written.
    //! @param [in] buff  The data to write.
    //! @param [in] sz    The number of elements to write.
    void write(const T *buff, const size_t sz)
    {
        if (m_Abort) return;

        std::unique_lock<std::mutex> lck(m_Mtx);

        try { m_WriteFunc(buff, sz, lck); } catch (int i) { }
    }

    //! Read data from the ring buffer. This will block until the requested amount is read.
    //! @param [in]  buff  The buffer to place the read data in.
    //! @param [in]  sz    The number of elements to read.
    void read(T *buff, const size_t sz)
    {
        if (m_Abort) return;

        std::unique_lock<std::mutex> lck(m_Mtx);

        try { m_ReadFunc(buff, sz, lck); } catch (int i) { }
    }

    //! Get the current diagnostic values.
    //! @param [out] d  Reference to a diagnotic structure.
    void diagnostics(diag &d)
    {
        std::unique_lock<std::mutex> lck(m_Mtx);
        d.fullCount = m_WaitForNotFullCount;
        d.emptyCount = m_WaitForAmtCount;
    }

    //! Return the current number of elements in the buffer
    //! @return The number of elements in the buffer.
    uint32_t amount()
    {
        std::unique_lock<std::mutex> lck(m_Mtx);
        return getAmount();
    }

    //! Set the ring_buffer instance to the abort state. This should only be
    //! called before destroying the instance as the state afterwards is
    //! considered invalid. Either the producer or consumer may call this but
    //! it's usually whichever side created the ring_buffer. This allows for
    //! a proper cleanup between the threads (e.g, using join()). Once in the abort
    //! state, any calls to *read()* or *write()* will have no effect.
    //! @param [in] code  The non-zero code describing the abort reason. This is
    //!                   not used by the ring_buffer instance other than to trigger
    //!                   an abort. The actual value can be used by the threads.
    //!                   Passing in zero in will have of effect.
    void abort(int code = -1)
    {
        std::unique_lock<std::mutex> lck(m_Mtx);
        m_Abort = code;
    }

    //! Get the current abort code.
    //! @return The abort code.
    int abortcode()
    {
        std::unique_lock<std::mutex> lck(m_Mtx);
        return m_Abort;
    }

private:

    using write_func = void(const T *buff, const size_t sz, std::unique_lock<std::mutex> &lck);
    using read_func  = void(T *buff, const size_t sz, std::unique_lock<std::mutex> &lck);

    std::function<write_func> m_WriteFunc;
    std::function<read_func>  m_ReadFunc;

    uint32_t    m_Capacity;
    uint32_t    m_Mask;
    uint32_t    m_YieldTime;

    std::unique_ptr<T[]>    m_DynamicBuff;
    T*                      m_StaticBuff;

    std::mutex m_Mtx;

    uint32_t m_WriteIdx;
    uint32_t m_ReadIdx;

    int m_Abort;

    uint32_t m_WaitForNotFullCount;
    uint32_t m_WaitForAmtCount;

    void dynamicWrite(const T *buff, const size_t sz, std::unique_lock<std::mutex> &lck)
    {
        for (size_t i=0;i < sz;i++)
        {
            waitNotFull(lck);
            m_DynamicBuff[m_WriteIdx++] = buff[i];
            m_WriteIdx &= m_Mask;
        }
    }

    void staticWrite(const T *buff, const size_t sz, std::unique_lock<std::mutex> &lck)
    {
        for (size_t i=0;i < sz;i++)
        {
            waitNotFull(lck);
            m_StaticBuff[m_WriteIdx++] = buff[i];
            m_WriteIdx &= m_Mask;
        }
    }

    void dynamicRead(T *buff, const size_t sz, std::unique_lock<std::mutex> &lck)
    {
        waitForAmount(sz, lck);

        for (size_t i=0;i < sz;i++)
        {
            buff[i] = m_DynamicBuff[m_ReadIdx++];
            m_ReadIdx &= m_Mask;
        }
    }

    void staticRead(T *buff, const size_t sz, std::unique_lock<std::mutex> &lck)
    {
        waitForAmount(sz, lck);

        for (size_t i=0;i < sz;i++)
        {
            buff[i] = m_StaticBuff[m_ReadIdx++];
            m_ReadIdx &= m_Mask;
        }
    }

    void waitNotFull(std::unique_lock<std::mutex> &lck)
    {
        if (!isFull())
            return;

        ++m_WaitForNotFullCount;
        while(1)
        {
            lck.unlock();
            timer::sleepUs(m_YieldTime);
            lck.lock();

            if (m_Abort)
                throw(m_Abort);

            if (!isFull())
                break;
        }
    }

    void waitForAmount(const size_t amt, std::unique_lock<std::mutex> &lck)
    {
        if (getAmount() >= amt)
            return;

        ++m_WaitForAmtCount;
        while(1)
        {
            lck.unlock();
            timer::sleepUs(m_YieldTime);
            lck.lock();

            if (m_Abort)
                throw(m_Abort);

            if (getAmount() >= amt)
                break;
        }
    }

    bool isFull()
    {
        return (((m_WriteIdx + 1) & m_Mask) == m_ReadIdx);
    }

    uint32_t getAmount()
    {
        return (m_WriteIdx >= m_ReadIdx) ? (m_WriteIdx - m_ReadIdx) : ((m_Capacity - m_ReadIdx) + m_WriteIdx);
    }
};
}
