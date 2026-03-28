// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <cstring>
#include <cassert>
#include <memory>
#include <iterator>
#include <cstddef>

#include "rm-math.h"

namespace util {

/*! \file aligned-ptr.h
 * \brief Container for an aligned buffer which may be required depending
 * on the system on which the system is running and the math library being used.
 * It is both movable and copyable and has random access iterator support.
 *
 * \note Static buffer instances are **NOT** copyable, but are movable. A runtime
 * assert is thrown if a copy is attempted.
 *
 * This is the main container used throughout *radiomon* to pass sample blocks
 * down the chains. You must use this when sourcing or sinking samples.
 * Using this frees the block implementations from having to ensure the buffers
 * it handles are properly aligned.
 *
 * You can also use it for any buffer requirements as long as it is for arithmetic or
 * complex types. This has similiar form to C++ smart pointers with the
 * one big difference being that *aligned_ptr* types have fixed sized buffers which
 * are determined at instantiation.
 *
 * See examples throughout the source code, especially in the *block* directory.
 *
 * \note Aligment of static buffers are **NOT** guaranteed. The caller is responsible
 * for ensuring this when creating the buffer if alignment is necessary.
 */

//! Helper functions to intialize an empty instance. You can call these mulitple times
//! to re-initialize if desired.
template<typename T>
class aligned_ptr;

//! Not for static buffer instances
template<typename T>
void init_aligned_ptr(aligned_ptr<T> &ap, const size_t size)
{
    static_assert(is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));
    if (!ap.m_IsStatic)
    {
        ap.del_ptr();
        ap.create_ptr(size);
        ap.m_Size = size;
    }
}

//! Not for static buffer instances
template<typename T>
void init_aligned_ptr(aligned_ptr<T> &ap, const size_t size, const T *values)
{
    static_assert(is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));
    if (!ap.m_IsStatic)
    {
        ap.del_ptr();
        ap.create_ptr(size);
        ap.m_Size = size;
        std::memcpy(ap.m_Ptr, values, size * sizeof(T));
    }
}

//! Not for static buffer instances
template<typename T>
void init_aligned_ptr_on_resize(aligned_ptr<T> &ap, const size_t size)
{
    static_assert(is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));
    if (!ap.m_IsStatic && (ap.size() != size))
    {
        ap.del_ptr();
        ap.create_ptr(size);
        ap.m_Size = size;
    }
}

//! Not for static buffer instances
template<typename T>
void init_aligned_ptr_on_resize(aligned_ptr<T> &ap, const size_t size, const T *values)
{
    static_assert(is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));
    if (!ap.m_IsStatic && (ap.size() != size))
    {
        ap.del_ptr();
        ap.create_ptr(size);
        ap.m_Size = size;
        std::memcpy(ap.m_Ptr, values, size * sizeof(T));
    }
}

//! Only for static buffer instances or to convert dynamic buffer instances
//! to a static buffer instance.
template<typename T>
void init_aligned_ptr_static(aligned_ptr<T> &ap, size_t size, const T *buff)
{
    ap.del_ptr();
    ap.m_Ptr = (T *)buff;
    ap.m_Size = size;
    ap.m_IsStatic = true;
}

template<typename T>
class aligned_ptr
{
    static_assert(is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));

public:

    //! Default contructor - creates an instance in the cleared state.
    //! Use one of the *init_aligned_ptr()* helpers to set things up when ready.
    aligned_ptr() : m_Ptr{nullptr}, m_Size{0}, m_IsStatic{false} { }

    //! parametric constuctor #1
    explicit aligned_ptr(const size_t size) : m_Ptr{nullptr}, m_Size{size}, m_IsStatic{false}
    {
        create_ptr(size);
    }

    //! parametric constructor #2
    aligned_ptr(const size_t size, const T *values) : m_Ptr{nullptr}, m_Size{size}, m_IsStatic{false}
    {
        create_ptr(size);
        std::memcpy(m_Ptr, values, size * sizeof(T));
    }

    //! destructor
    ~aligned_ptr(){ del_ptr(); }

    //! copy constructor
    //! \note static buffer aligned_ptrs cannot be copied
    aligned_ptr(const aligned_ptr &other)
    {
        assert(!other.m_IsStatic);

        m_Size = other.m_Size;
        m_IsStatic = other.m_IsStatic;
        create_ptr(m_Size);
        std::memcpy(m_Ptr, other.m_Ptr, m_Size * sizeof(T));
    }

    //! copy assignment constructor
    //! \note Static buffer aligned_ptrs cannot be copied
    aligned_ptr& operator=(const aligned_ptr &other)
    {
        assert(!other.m_IsStatic);

        if (m_Ptr == other.m_Ptr)
            return *this;

        m_Size = other.m_Size;
        m_IsStatic = other.m_IsStatic;

        create_ptr(m_Size);
        std::memcpy(m_Ptr, other.m_Ptr, m_Size * sizeof(T));

        return *this;
    }

    //! move constructor
    aligned_ptr(aligned_ptr &&other)
    {
        m_Ptr = std::exchange(other.m_Ptr, nullptr);
        m_Size = std::exchange(other.m_Size, 0);
        m_IsStatic = std::exchange(other.m_IsStatic, false);
    }

    //! move assignment constructor
    aligned_ptr& operator=(aligned_ptr &&other)
    {
        if (m_Ptr == other.m_Ptr)
            return *this;

        del_ptr();
        m_Ptr = std::exchange(other.m_Ptr, nullptr);
        m_Size = std::exchange(other.m_Size, 0);
        m_IsStatic = std::exchange(other.m_IsStatic, false);

        return *this;
    }

    //! op overload for array style index reading (rhs)
    const T& operator[](size_t i) const
    {
        if (i < m_Size)
            return m_Ptr[i];

        return m_Ptr[0];
    }

    //! op overload for array style index writing (lhs)
    T& operator[](size_t i)
    {
        if (i < m_Size)
            return m_Ptr[i];

        return m_Ptr[0];
    }

    //! Get the pointer of the contained buffer.
    const T* data() const
    {
        return m_Ptr;
    }

    //! Get the size of the contained buffer.
    const size_t size() const
    {
        return m_Size;
    }

    //! Check if the instance has a null pointer (e.g., in a cleared state).
    const bool empty() const
    {
        return (bool)!m_Ptr;
    }

    //! Reset the instance to a default contructed state
    void clear()
    {
        del_ptr();
    }

    //! Iterator which *should* support algorithms requiring random access.
    struct iterator
    {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        iterator() = delete;

        // destructible requirement
        ~iterator() = default;

        iterator(T *ptr) : m_iPtr(ptr) { }

        // copy contructible requirement
        iterator(const iterator &other)
        {
            m_iPtr = other.m_iPtr;
        }

        // copy assignable requirement
        iterator& operator=(const iterator &other) const
        {
            if (m_iPtr == other.m_iPtr)
                return *this;

            m_iPtr = other.m_iPtr;
            return *this;
        }

        // swappable requirment
        iterator(aligned_ptr &&other)
        {
            std::exchange(other.m_iPtr, nullptr);
        }

        // swappable requirment
        iterator& operator=(iterator &&other)
        {
            if (m_iPtr == other.m_iPtr)
                return *this;

            m_iPtr = std::exchange(other.m_iPtr, nullptr);
            return *this;
        }

        // LegacyInputIterator, LegacyOutputIterator, and random access expression requirements
        T& operator*() const { return *m_iPtr; }
        T* operator->() { return m_iPtr; }
        T operator[](int i) const { return m_iPtr[i]; }
        T operator*(T value) const { *m_iPtr = value; }
        iterator operator++(int) { return iterator(m_iPtr++); }

        iterator& operator--()
        {
            --m_iPtr;
            return *this;
        }

        iterator operator--(int){ iterator(m_iPtr--); }

        iterator& operator++()
        {
            ++m_iPtr;
            return *this;
        }

        iterator& operator+=(const int n)
        {
            m_iPtr += n;
            return *this;
        }

        iterator operator+(const int n) { return (m_iPtr + n); }
        iterator& operator-=(const int n)
        {
            m_iPtr -= n;
            return *this;
        }

        iterator operator-(const int n) const { return iterator(m_iPtr - n); }

        // Not sure what category this falls into but it's needed for
        // compatiblity with GNU STL at least (e.g., std::move() from algorithm)
        int operator-(const iterator &other) const { return (m_iPtr - other.m_iPtr); }

        // equality comparable requirment
        friend bool operator==(const iterator &a, const iterator &b){ return a.m_iPtr == b.m_iPtr; }
        friend bool operator!=(const iterator &a, const iterator &b){ return a.m_iPtr != b.m_iPtr; }

    private:
        T* m_iPtr;
    };

    iterator begin() const { return iterator(m_Ptr); }
    iterator end() const{ return iterator(&m_Ptr[m_Size]); }

private:

    void del_ptr()
    {
        if (!m_IsStatic)
        {
            if (m_Ptr)
            {
                rm_math::rm_free(m_Ptr);
                m_Ptr = nullptr;
            }

            m_Size = 0;
        }
    }

    void create_ptr(size_t size)
    {
        if (!m_IsStatic)
            m_Ptr = rm_math::rm_malloc<T>(size * sizeof(T));
    }

    friend void init_aligned_ptr<>(aligned_ptr<T> &ap, const size_t size);
    friend void init_aligned_ptr<>(aligned_ptr<T> &ap, const size_t size, const T *values);
    friend void init_aligned_ptr_on_resize<>(aligned_ptr<T> &ap, const size_t size);
    friend void init_aligned_ptr_on_resize<>(aligned_ptr<T> &ap, const size_t size, const T *values);
    friend void init_aligned_ptr_static<>(aligned_ptr<T> &ap, size_t size, const T *buff);

    // RAM usage
    T* m_Ptr;
    size_t m_Size;
    bool m_IsStatic;
};

//! Helper functions to create an instance on the heap. If you want to initialize
//! an empty instance (default constructed), use one of the init_aligned_ptr()
//! functions at the top of this file which are more efficient than instantiating
//! and moving a new instance.
template<typename T>
aligned_ptr<T> make_aligned_ptr(const size_t size)
{
    static_assert(is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));
    return std::move(*(new aligned_ptr<T>(size)));
}

template<typename T>
aligned_ptr<T> make_aligned_ptr(const size_t size, const T *values)
{
    static_assert(is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));
    return std::move(*(new aligned_ptr<T>(size, values)));
}

template<typename T>
aligned_ptr<T> make_aligned_ptr_static(const size_t size, const T *buffer)
{
    static_assert(is_std_complex_v<T> || (std::is_arithmetic<T>::value == std::true_type()));
    auto *p = new aligned_ptr<T>();
    init_aligned_ptr_static<T>(*p, size, buffer);

    return std::move(*p);
}
}