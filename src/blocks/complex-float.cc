// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "complex-float.h"

using namespace dsp;

complex_float::complex_float() : block<dsp::func_cf> { TYPE_OPERATOR }
{
    block<dsp::func_cf>::process = std::bind(&complex_float::convert, this, std::placeholders::_1, std::placeholders::_2);
}

void complex_float::convert(const util::aligned_ptr<rm_math::complex_f> &inBlock, util::aligned_ptr<float> &outBlock)
{
    util::init_aligned_ptr_on_resize<float>(outBlock, inBlock.size());

    for (size_t i=0;i < inBlock.size();i++)
        outBlock[i] = inBlock[i].real();
}
