// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <type_traits>
#include <complex>

#include "block.h"

namespace dsp { namespace endpoints {

/*! \brief Simple Callback Sink
 *
 * This endpoint simply takes the result of a chain's operators and passes it to
 * the supplied callback. It does no processing on the actual data.
 */

template<typename T, typename B>
class callback : public block<B>
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);
    static_assert(is_block_func_v<B>);

public:

    //! The callback method signature.
    using user_cb = void(const util::aligned_ptr<T> &data);

    //! Create an instance.
    //! @param [in] cb  The method to invoke when a chain iteration is complete.
    callback(user_cb cb) : block<B> { TYPE_SINK }
    {
        m_CB = cb;
        block<B>::process = std::bind(&callback::cb_wrapper, this, std::placeholders::_1, std::placeholders::_2);
    }

    //! Invoke the callback method.
    //! @param [in]  inBlock     The data to send to the method
    //! @param [out] outBlock    Ignored. This will be removed in a future commit.
    void cb_wrapper(const util::aligned_ptr<T> &inBlock, util::aligned_ptr<T> &outBlock)
    {
        m_CB(inBlock);
    }

private:

    user_cb *m_CB;

};

using callback_ff = callback<float, func_ff>;
using callback_cc = callback<rm_math::complex_f, func_cc>;

}}
