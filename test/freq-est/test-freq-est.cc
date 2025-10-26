// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdio.h>
#include <complex>

#include "freq-est.h"
#include "cmdline.h"
#include "sine-source.h"
#include "aligned-ptr.h"

constexpr float Fs          = 48000.0f;
constexpr float Hz          = 12500.0f;
constexpr float freq        = (2*M_PI*Hz/Fs);
constexpr size_t sigSize    = 128;

int main(int argc, char **argvp)
{
    auto sigBuff = util::make_aligned_ptr<util::complex_f>(sigSize);
    util::sine_source<util::complex_f> sig{ freq };
    util::freq_est est{ Fs/(2*M_PI) };

    sig.get(sigBuff);
    printf("estimate = %06f\n", est.estimate(sigBuff));

    sig.setFrequency(freq / 2.0f);
    sig.get(sigBuff);
    printf("estimate = %06f\n", est.estimate(sigBuff));

    sig.get(sigBuff);
    printf("estimate = %06f\n", est.estimate(sigBuff));
    return 0;
}
