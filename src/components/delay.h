// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <type_traits>
#include <array>
#include <functional>

#include "aligned-ptr.h"

namespace comps {

/*! \brief Simple one sample delay block.
 *
 */

template <typename T, size_t capacity = 1>
class delay
{
    static_assert(util::is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));
    static_assert(capacity > 1);

public:

    delay(const delay &) = delete;
    delay& operator=(const delay &) = delete;

    delay(delay &&) = delete;
    delay& operator=(delay &&) = delete;

    delay()
    {
        util::init_aligned_ptr(m_DelayPtr, capacity);
        std::fill(m_DelayPtr.begin(), m_DelayPtr.end(), 0);
        m_ShiftFunc = std::bind(&delay::shiftVect,  this, std::placeholders::_1);
        m_GetFunc = std::bind(&delay::getVect, this);
        m_SizeFunc = std::bind(&delay::sizeVect, this);
    }

    delay(std::array<T, capacity> &arr) : m_DelayArr { arr }
    {
        m_ShiftFunc = std::bind(&delay::shiftArr, this, std::placeholders::_1);
        m_GetFunc = std::bind(&delay::getArr, this);
        m_SizeFunc = std::bind(&delay::sizeArr, this);
    }

    //! Insert a new value into the block
    //! @param [in] newValue The value to insert.
    //! @return     The currently stored value.
    T operator<<(const T newValue)
    {
        return m_ShiftFunc(newValue);
    }

    //! Get the current value stored in the block
    T get()
    {
        return m_GetFunc();
    }

    //! Get number of items in the shift buffer
    size_t size()
    {
        return m_SizeFunc();
    }

private:

    //! \cond

    std::function<T(const T)>    m_ShiftFunc;
    std::function<T()>           m_GetFunc;
    std::function<size_t()>      m_SizeFunc;

    util::aligned_ptr<T>         m_DelayPtr;
    std::array<T, capacity>      m_DelayArr;

    T shiftVect(const T newValue)
    {
        T res = m_DelayPtr.back();
        std::move_backward(m_DelayPtr.begin(), m_DelayPtr.end() - 1, m_DelayPtr.end());
        m_DelayPtr[0] = newValue;

        return res;
    }

    T getVect()
    {
        return m_DelayPtr.back();
    }

    T shiftArr(const T newValue)
    {
        T res = m_DelayArr.back();
        std::move_backward(m_DelayArr.begin(), m_DelayArr.end() - 1, m_DelayArr.end());
        m_DelayArr[0] = newValue;

        return res;
    }

    T getArr()
    {
        return m_DelayArr.back();
    }

    size_t sizeArr()
    {
        return m_DelayArr.size();
    }

    size_t sizeVect()
    {
        return m_DelayPtr.size();
    }

    //! \endcond
};

// Partial specialization for a one sample block.
template<typename T>
class delay<T, 1>
{
    static_assert(util::is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));

public:

    delay() : m_Value { 0 } { }

    T operator<<(const T &newValue)
    {
        T res = m_Value;
        m_Value = newValue;
        return res;
    }

    T get()
    {
        return m_Value;
    }

    size_t size()
    {
        return 1;
    }

private:
    T m_Value;
};

}
