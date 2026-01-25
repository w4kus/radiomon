// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <complex>
#include <stdio.h>
#include <cmath>
#include <memory>
#include <volk/volk.h>

#include "nco.h"
#include "loop-filter.h"
#include "cmdline.h"
#include "sine-source.h"

constexpr size_t NUM_SAMPLES(256);

int main(int argc, char **argvp)
{
    auto blk = util::make_aligned_ptr<rm_math::complex_f>(NUM_SAMPLES);
    auto out = util::make_aligned_ptr<rm_math::complex_f>(NUM_SAMPLES);
    auto sig = util::sine_source<rm_math::complex_f>(2 * M_PI / NUM_SAMPLES, M_PI / 2.0f);
    auto nco = comps::nco(1.0f);
    auto lf = comps::loopfilt(0.2667f, 0.01778f);
    float error = 0.0f;

    // Input signal
    FILE *f = fopen("test-nco-input.txt", "w");
    sig.get(blk);
    util::printComplex(f, blk.size(), blk.get());
    fclose(f);

    nco.setFrequency(2 * M_PI / NUM_SAMPLES);

    for (size_t i = 0; i < NUM_SAMPLES; i++)
    {
        // Get the sample adjusted by the error and store it in the
        // output buffer
        auto ferror = lf.filter(error);
        auto tmp = nco.adjustPhase(ferror);

        out[i] = tmp;
        error = rm_math::atan2(blk[i] * std::conj(tmp));

        printf("%.6f ", ferror);
    }

    f = fopen("test-nco-output.txt", "w");
    util::printComplex(f, NUM_SAMPLES, out.get());
    fclose(f);

    return 0;
}
