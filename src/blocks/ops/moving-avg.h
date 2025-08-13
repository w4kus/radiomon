// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <cstring>
#include <type_traits>
#include <memory>

namespace dsp {

/*! \brief Moving average filter.
 *
 * This provides a low pass filter in the form of a moving average.
 *
 */

template<typename T>
class average
{
    static_assert(std::is_floating_point<T>::value == true);

public:
    average() = delete;
    average(const average &) = delete;
    average& operator= (const average&) = delete;

    //! Create an instance with the number of elements of the filter.
    //! @param [in] numElements The maximum number of values to average.
    //! The storage is initialized to zero upon instantiation.
    average(size_t numElements) : m_Size(numElements),
                                    m_h(0.0),
                                    m_Idx(0)
    {
        m_Data = std::make_unique<T[]>(numElements);
        m_h = 1.0 / numElements;
        std::memset(m_Data.get(), 0, numElements * sizeof(T));
    }

    //! Insert a new sample into the moving average filter.
    //! @param [in] elem The value to insert.
    //! @return The average after insertion of the new value.
    //! \note At the start, the average will include zeros until the full number
    //! of elements have been inserted.
    T insert(const float elem)
    {
        T avg = 0;

        m_Data[m_Idx++] = elem * m_h;

        if (m_Idx == m_Size)
            m_Idx = 0;

        for (size_t i=0;i < m_Size;i++)
            avg += m_Data[i];

        return avg;
    }

private:

    size_t m_Size;
    T m_h;
    size_t m_Idx;

    std::unique_ptr<T[]> m_Data;
};

}
