// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "carrier-sync.h"
#include "cmdline.h"
#include "sine-source.h"
#include "aligned-ptr.h"

constexpr size_t NUM_SAMPLES(4196);
constexpr int SAMPLE_RATE(48000);
constexpr int HZ(4000);
constexpr float FREQUENCY(2 * M_PI * (float)HZ / (float)SAMPLE_RATE);

int main(int argc, char **argvp)
{
    auto sig = util::sine_source<rm_math::complex_f>(FREQUENCY); // , M_PI / 2);
    auto in = util::make_aligned_ptr<rm_math::complex_f>(NUM_SAMPLES);
    auto out = util::aligned_ptr<rm_math::complex_f> { };
    dsp::carrier_sync cs(0.062831853f, 0.031415927f, 2);

    sig.get(in);

    cs.setSamplingRate(SAMPLE_RATE);
    cs.sync(in, out);

    FILE *f = fopen("test-carrier-sync-out.txt", "w");
    util::printComplex(f, out.size(), out.get());
    fclose(f);

    f = fopen("test-carrier-sync-in.txt", "w");
    util::printComplex(f, in.size(), in.get());
    fclose(f);

    auto err = util::aligned_ptr<float> { };
    cs.getErrorSig(err);

    f = fopen("test-carrier-sync-error.txt", "w");
    util::printReal(f, err.size(), err.get());
    fclose(f);

    return 0;
}
