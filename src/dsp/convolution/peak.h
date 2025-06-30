#pragma once

#include <cmath>
#include <type_traits>

namespace util {

template<typename T>
class peak
{
    static_assert(std::is_signed<T>::value == true);

public:
    peak() : m_Cnt(0), 
             m_Syms{ {0, 0}, {0, 0}, {0, 0} } {}

    bool get(const T sym, T &pk)
    {
        bool ret = false;

        m_Syms[2] = m_Syms[1];
        m_Syms[1] = m_Syms[0];

        m_Syms[0].val = sym;
        m_Syms[0].absVal = std::abs(sym);

        ++m_Cnt;

        if (m_Cnt == 3)
        {
            if ((m_Syms[0].absVal < m_Syms[1].absVal) && (m_Syms[2].absVal < m_Syms[1].absVal))
            {
                pk = m_Syms[1].val;
                ret = true;
            }

            m_Cnt = 1;
        }

        return ret;
    }

private:
    struct sym_t
    {
        T val;
        T absVal;
    };

    uint8_t m_Cnt;
    sym_t m_Syms[3];
};
}
