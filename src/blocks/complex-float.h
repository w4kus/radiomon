// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include "block.h"

namespace dsp {

/*! \brief Complex To Float Block
 *
 * This block converts a complex signal to a real signal by taking the real part of the complex samples.
 * This is useful for applications such as demodulation where the output is a real signal.
 * The imaginary part of the complex samples is discarded.
 *
 */

class complex_float : public block<dsp::func_cf>
{

public:
    //! Create an instance of the complex to float block.
    complex_float();

    //! Convert a block of complex samples to a block of real samples.
    //! @param [in]  inBlock     The block of complex samples to convert.
    //! @param [out] outBlock    The block of real samples which will be the same size as *inBlock*.
    void convert(const util::aligned_ptr<rm_math::complex_f> &inBlock, util::aligned_ptr<float> &outBlock);
};

}
