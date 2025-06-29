#pragma once

#include <cstring>
#include <type_traits>

namespace dsp {

template<typename T>
class average
{
    static_assert(std::is_floating_point<T>::value == true);

public:
    average() = delete;
    average(const average &) = delete;
    average& operator= (const average&) = delete;

    average(size_t numElements) : m_Size(numElements),
                                    m_h(0.0),
                                    m_Idx(0)
    {
        m_Data = std::make_unique<T[]>(numElements);
        m_h = 1.0 / numElements;
        std::memset(m_Data.get(), 0, numElements * sizeof(T));
    }

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
