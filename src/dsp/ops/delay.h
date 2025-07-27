// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

namespace dsp
{

/*! \brief Delay block
 *
 * A small template to implement single sample delays.
 *
*/

template<typename T>
class delay
{
public:

    delay() = delete;

    //! Create an instance.
    //! @param [in] initRet The value returned when given the first sample.
    //! Default to zero.
    delay(const T initRet = 0) : z(initRet) {}

    //! Insert a new sample into the delay block.
    //! @param [in] sample The new sample.
    //! @return The current sample in the delay block.
    T ins(const T sample)
    {
        auto ret = z;
        z = sample;
        return ret;
    }

    //! Return the current sample in the delay block.
    //! @return The current sample in the delay block.
    T get() { return z; }

private:
    T z;
};

}
