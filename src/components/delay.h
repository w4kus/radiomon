// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <type_traits>
#include "aligned-ptr.h"

namespace comps {

/*! \brief Simple one sample delay block.
 *
 */

template <typename T>
class delay
{
    static_assert(util::is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));

public:

    delay() : m_Value { 0 } { }

    //! Insert a new value into the block
    //! @param [in] newValue The value to insert.
    //! @return     The currently stored value.
    T operator<<(const T &newValue)
    {
        T res = m_Value;
        m_Value = newValue;
        return res;
    }

    //! Get the current value stored in the block
    T get()
    {
        return m_Value;
    }

private:
    T m_Value;
};

}
