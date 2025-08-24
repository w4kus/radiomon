// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <cstring>
#include <cassert>
#include <volk/volk.h>
#include <memory>

namespace util {

 template<typename T>
 struct is_std_complex : std::false_type { };

 template<typename T>
 struct is_std_complex<std::complex<T>> : std::true_type { };

/*! \brief Container for an aligned buffer compatible with the
 * [Volk](https://www.libvolk.org/) Libray. It is both movable and copyable.
 *
 * This is the main container used throughout *dmr-mon* to pass sample blocks
 * down the chain through dsp blocks. You must use this when sourcing or sinking samples.
 * Using this frees the block implementations from having to ensure the buffers
 * it handles are properly aligned.
 *
 * You can also use it for any buffer requirements as it supports arithmetic and
 * complex types. This is a good idea since it means the buffer can be easily
 * inserted into a chain if needed. This has similiar form to *std::unique_ptr* and
 * *std::shared_ptr* containers. See examples throughout the source code, especially in
 * the *block* directory.
 *
 * \note Support for copying is included mainly for block constructors to have the
 * ability to easily copy data for such things as filter taps. This also means you
 * could develop a block which allows pass by value for sample handlers in a chain.
 * Please avoid this and **always** pass sample data by a **const** reference to
 * sample processing methods. In fact, all *all* parameters, including *aligned_ptr*
 * types should be of const.
 */

template<typename T>
class aligned_ptr
{
    static_assert((is_std_complex<T>::value == true) || (std::is_arithmetic<T>::value == true));

public:

    //! default contructor - creates an instance in the cleared state.
    //! Use one of the *make_aligned_ptr()* helpers at the bottom
    //! to set things up when ready.
    aligned_ptr() : m_Ptr{nullptr}, m_Size{0} { }

    //! parametric constuctor #1
    aligned_ptr(const size_t size) : m_Ptr{nullptr}, m_Size{size}
    {
        create_ptr(size);
    }

    //! parametric constructor #2
    aligned_ptr(const size_t size, const T *values) : m_Ptr{nullptr}, m_Size{size}
    {
        create_ptr(size);
        std::memcpy(m_Ptr, values, size * sizeof(T));
    }

    //! destructor
    ~aligned_ptr(){ del_ptr(); }

    // copy constructor
    aligned_ptr(const aligned_ptr &other)
    {
        m_Size = other.m_Size;
        create_ptr(m_Size);
        std::memcpy(m_Ptr, other.m_Ptr, m_Size * sizeof(T));
    }

    //! copy assignment constructor
    aligned_ptr& operator=(const aligned_ptr &other)
    {
        if (m_Ptr == other.m_Ptr)
            return *this;

        m_Size = other.m_Size;

        del_ptr();
        create_ptr(m_Size);
        std::memcpy(m_Ptr, other.m_Ptr, m_Size * sizeof(T));

        return *this;
    }

    //! move constructor
    aligned_ptr(aligned_ptr &&other)
    {
        std::swap(m_Ptr, other.m_Ptr);
        std::swap(m_Size, other.m_Size);
    }

    //! move assignment constructor
    aligned_ptr& operator=(aligned_ptr &&other)
    {
        if (m_Ptr == other.m_Ptr)
            return *this;

        del_ptr();
        m_Ptr = std::exchange(other.m_Ptr, nullptr);
        m_Size = std::exchange(other.m_Size, 0);

        return *this;
    }

    //! op overload for array style index reading
    const T& operator[](size_t i) const
    {
        if (i < m_Size)
            return m_Ptr[i];

        assert(1 && "aligned_buffer: index out of range");
        return m_Ptr[m_Size - 1];
    }

    //! op overload for array style index writing
    T operator [](size_t i)
    {
        if (i < m_Size)
            return m_Ptr[i];

        assert(1 && "aligned_buffer: index out of range");
        return -99;
    }


    //! Get the pointer of the contained buffer.
    const T* get() const
    {
        return m_Ptr;
    }

    //! Get the number of elements in the contained buffer.
    const size_t size() const
    {
        return m_Size;
    }

    //! Check if the instance has a null pointer (e.g., in a cleared state).
    const bool invalid() const
    {
        return (bool)!m_Ptr;
    }

private:

    void del_ptr() { if (m_Ptr) volk_free(m_Ptr); }
    void create_ptr(size_t size) { m_Ptr = (T *)volk_malloc(size * sizeof(T), volk_get_alignment()); }

    T* m_Ptr;
    size_t m_Size;
};

// Helper functions to create an instance using move semantics
template<typename T>
aligned_ptr<T> make_aligned_ptr(const size_t size)
{
    return std::move(*(new aligned_ptr<T>(size)));
}

template<typename T>
aligned_ptr<T> make_aligned_ptr(const size_t size, const T *values)
{
    return std::move(*(new aligned_ptr<T>(size, values)));
}

}
