#pragma once

#include <mutex>
#include <cmath>
#include <exception>
#include <thread>
#include <memory>

namespace util {

class ringbuffexception : public std::exception
{
    const char *m;

public:
    ringbuffexception(const char *msg) : m(msg) {}

    auto what() { return m; }
};

template <typename T>
class ring_buffer
{
public:
    ring_buffer() = delete;
    ring_buffer(const ring_buffer&) = delete;
    ring_buffer& operator= (const ring_buffer&) = delete;

    ring_buffer(uint8_t exp, uint32_t yieldTime = 500) :  
                m_Size(0),
                m_YieldTime(yieldTime),
                m_WriteIdx(0),
                m_ReadIdx(0),
                m_Break(false)
    {
        if (exp < 25)
        {
            m_Capacity = (size_t)std::pow(2, exp);
            m_Mask = m_Capacity - 1;
            m_RingBuff = std::make_unique<T[]>(m_Capacity);
        }
        else
            throw ringbuffexception("ring_buffer: Illegal exponent value\n");
    }

    void push(T val)
    {
        std::unique_lock<std::mutex> lck(m_Mtx);
        waitNotFull(lck);

        if (m_Break) return;

        m_RingBuff[m_WriteIdx++] = val;
        m_WriteIdx &= m_Mask;
        ++m_Size;
    }

    void pushBuffer(T *buff, size_t sz)
    {
        for (size_t i=0;i < sz;i++)
        {
            push(buff[i]);

            if (m_Break) return;
        }
    }

    T pop()
    {
        std::unique_lock<std::mutex> lck(m_Mtx);
        waitNotEmpty(lck);

        if (m_Break) return 0;

        auto ret = m_RingBuff[m_ReadIdx++];
        m_ReadIdx &= m_Mask;
        --m_Size;
        return ret;
    }

    size_t getSize() { return m_Size; }

    void setBreak() { m_Break = true; }
    void clearBreak() { m_Break = false; }

private:
    uint32_t m_Mask;
    uint32_t m_Size;
    uint32_t m_Capacity;
    uint32_t m_YieldTime;

    std::unique_ptr<T[]> m_RingBuff;
    std::mutex m_Mtx;

    int m_WriteIdx;
    int m_ReadIdx;

    bool m_Break;

    void waitNotFull(std::unique_lock<std::mutex> &lk)
    {
        while(1)
        {
            if (m_Break || (m_Size < m_Capacity))
                break;

            lk.unlock();
            std::this_thread::sleep_for(std::chrono::microseconds(m_YieldTime));
            lk.lock();
        }
    }

    void waitNotEmpty(std::unique_lock<std::mutex> &lk)
    {
        while(1)
        {
            if (m_Break || (m_Size > 0))
                break;

            lk.unlock();
            std::this_thread::sleep_for(std::chrono::microseconds(m_YieldTime));
            lk.lock();
        }
    }
};
}
