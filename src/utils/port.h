// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <type_traits>
#include <mutex>
#include <deque>

#include "aligned-ptr.h"
#include "rm-math.h"

namespace util {

/*! \brief Generic buffer for sending and receiving Data
 *
 * This implements a buffering mechanism to allow modules (e.g., blocks and components)
 * to send data to other modules. The primary purpose is for testing and debugging
 * modules. This is designed as a one-to-one pipe with a *producer* (e.g., the module sending
 * the data), and the *consumer* (e.g., the module receiving the data).
 * 
 * This is thread safe so the producer and consumber need not execute
 * in the same thread. This can be useful if you use *GNU Radio* to test your blocks. The unit of
 * data is **blocks** rather than individual samples so contructing a port of size **N** implies
 * that the queue will hold **N** blocks of samples of varying lengths.
 */

template<typename T>
class port
{
    static_assert((std::is_arithmetic<T>::value == std::true_type()) || util::is_std_complex_v<T>);

public:

    port() = delete;
    port(const port&) = delete;
    port& operator=(const port&) = delete;
    port(const port&&) = delete;
    port& operator=(const port&&) = delete;

    //! Construct a port.
    //! @param [in] size The maximum number of sample blocks implemented using a standard *deque* container.
    port(const size_t size) : m_MaxSize { size }
    { 
    }

    //! Send a block to the port.
    //! @param [in] samples The block of samples to insert into the queue. Note that this uses move semantics.
    void produce(aligned_ptr<T> &&samples)
    {
        if (m_MaxSize)
        {
            std::unique_lock<std::mutex> lck(m_Mtx);

            if (m_SampleBlockList.size() == m_MaxSize)
                m_SampleBlockList.pop_back();

            m_SampleBlockList.push_front(std::move(samples));
        }
        else
            samples.reset();
    }

    //! Read the block of data at the queue tail. 
    //! @param [out] samples An empty *aligned_ptr* type. Note that this uses move semantics.
    void consume(aligned_ptr<T> &samples)
    {
        if (m_SampleBlockList.size())
        {
           std::unique_lock<std::mutex> lck(m_Mtx);

           samples = std::move(m_SampleBlockList.back());
           m_SampleBlockList.pop_back();
        }
    }

    //! Get the current size of the block queue.
    //! @return The number of blocks in the queue.
    size_t size() const
    {
        return m_SampleBlockList.size();
    }

    //! Get the maximum size of the block queue.
    //! @return The maximum number of blocks of the queue.
    size_t maxSize() const
    {
        return m_MaxSize;
    }

private:

    size_t m_MaxSize;

    std::mutex m_Mtx;
    std::deque<aligned_ptr<T>> m_SampleBlockList;
};
}
