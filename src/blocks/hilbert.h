// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include "block.h"

namespace dsp {

/*! \brief Hilbert Transform
 *
 * This is the beginning of a Hilbert transform. Currently it is not implemented
 * but included for some tests. More to come.
 */

class hilbert : public block<dsp::func_fc>
{

public:
    //! Create an instance of the Hilbert transform block.
    hilbert();

    //! Apply the Hilbert transform to a block of real samples to produce a block of complex samples.
    //! @param [in]  inBlock     The block of real samples to transform.
    //! @param [out] outBlock    The block of complex samples which will be the same size as *inBlock*.
    void transform(const util::aligned_ptr<float> &inBlock, util::aligned_ptr<rm_math::complex_f> &outBlock);
};
}
