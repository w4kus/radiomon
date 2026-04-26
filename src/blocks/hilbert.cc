// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "hilbert.h"

using namespace dsp;

hilbert::hilbert() : block<dsp::func_fc> { TYPE_OPERATOR }
{
    block<dsp::func_fc>::process = std::bind(&hilbert::transform, this, std::placeholders::_1, std::placeholders::_2);
}

void hilbert::transform(const util::aligned_ptr<float> &inBlock, util::aligned_ptr<rm_math::complex_f> &outBlock)
{
    util::init_aligned_ptr_on_resize<rm_math::complex_f>(outBlock, inBlock.size());

    // FIXME - just for testing
    for (size_t i=0;i < inBlock.size();i++)
        outBlock[i] = rm_math::complex_f(inBlock[i], 0.0f);
}
