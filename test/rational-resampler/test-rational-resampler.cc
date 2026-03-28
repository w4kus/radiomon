// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdio.h>
#include <complex>
#include <array>

// #include "poly-subfilter.h"
#include "rational-resampler.h"
#include "cmdline.h"
#include "aligned-ptr.h"
#include "sine-source.h"
#include "trace.h"

// Non-decomposed filter
#if 0
const float lp_hamming_8p5K_48k[65] =
{
	-0.000645, 0.000135, 0.000898, 0.000754, -0.000445, -0.001570, -0.001046, 0.001195,
    0.002836, 0.001296, -0.002716, -0.004727, -0.001152, 0.005367, 0.007170, 0.000123,
    -0.009549, -0.009988, 0.002475, 0.015807, 0.012921, -0.007732, -0.025188, -0.015658,
    0.017936, 0.040562, 0.017889, -0.040379, -0.073915, -0.019348, 0.125724, 0.284391,
    0.353157, 0.284391, 0.125724, -0.019348, -0.073915, -0.040379, 0.017889, 0.040562,
    0.017936, -0.015658, -0.025188, -0.007732, 0.012921, 0.015807, 0.002475, -0.009988,
    -0.009549, 0.000123, 0.007170, 0.005367, -0.001152, -0.004727, -0.002716, 0.001296,
    0.002836, 0.001195, -0.001046, -0.001570, -0.000445, 0.000754, 0.000898, 0.000135,
    -0.000645
};

#endif

constexpr int F                     = 1000;
constexpr int Fs                    = 48000;
constexpr size_t SIG_CYCLE_NUM      = 256;
constexpr size_t SIG_SAMPLE_SIZE    = Fs/F*SIG_CYCLE_NUM;

// Decomposed filter
#if 0
const float lp_hamming_8p5K_48k[4][17] =
{
	{ -0.000645, -0.000445, 0.002836, -0.001152, -0.009549, 0.012921, 0.017936, -0.073915, 0.353157, -0.073915, 0.017936, 0.012921, -0.009549, -0.001152, 0.002836, -0.000445, -0.000645 },
	{ 0.000135, -0.001570, 0.001296, 0.005367, -0.009988, -0.007732, 0.040562, -0.019348, 0.284391, -0.040379, -0.015658, 0.015807, 0.000123, -0.004727, 0.001195, 0.000754, 0.000000 },
	{ 0.000898, -0.001046, -0.002716, 0.007170, 0.002475, -0.025188, 0.017889, 0.125724, 0.125724, 0.017889, -0.025188, 0.002475, 0.007170, -0.002716, -0.001046, 0.000898, 0.000000 },
	{ 0.000754, 0.001195, -0.004727, 0.000123, 0.015807, -0.015658, -0.040379, 0.284391, -0.019348, 0.040562, -0.007732, -0.009988, 0.005367, 0.001296, -0.001570, 0.000135, 0.000000 }
};
#endif

// #define USE_VECTOR

#ifdef USE_VECTOR
// Taps dynamically allocated on the heap
const std::vector<std::vector<float>> lp_poly_hamming_8p5K_48k =
{
	{ -0.000645, -0.000445, 0.002836, -0.001152, -0.009549, 0.012921, 0.017936, -0.073915, 0.353157, -0.073915, 0.017936, 0.012921, -0.009549, -0.001152, 0.002836, -0.000445, -0.000645 },
	{ 0.000135, -0.001570, 0.001296, 0.005367, -0.009988, -0.007732, 0.040562, -0.019348, 0.284391, -0.040379, -0.015658, 0.015807, 0.000123, -0.004727, 0.001195, 0.000754, 0.000000 },
	{ 0.000898, -0.001046, -0.002716, 0.007170, 0.002475, -0.025188, 0.017889, 0.125724, 0.125724, 0.017889, -0.025188, 0.002475, 0.007170, -0.002716, -0.001046, 0.000898, 0.000000 },
	{ 0.000754, 0.001195, -0.004727, 0.000123, 0.015807, -0.015658, -0.040379, 0.284391, -0.019348, 0.040562, -0.007732, -0.009988, 0.005367, 0.001296, -0.001570, 0.000135, 0.000000 }
};

#else
// Taps statically allocated
const std::array<std::array<float, 17>, 4> lp_poly_hamming_8p5K_48k =
{{
	{ -0.000645, -0.000445, 0.002836, -0.001152, -0.009549, 0.012921, 0.017936, -0.073915, 0.353157, -0.073915, 0.017936, 0.012921, -0.009549, -0.001152, 0.002836, -0.000445, -0.000645 },
	{ 0.000135, -0.001570, 0.001296, 0.005367, -0.009988, -0.007732, 0.040562, -0.019348, 0.284391, -0.040379, -0.015658, 0.015807, 0.000123, -0.004727, 0.001195, 0.000754, 0.000000 },
	{ 0.000898, -0.001046, -0.00276, 0.007170, 0.002475, -0.025188, 0.017889, 0.125724, 0.125724, 0.017889, -0.025188, 0.002475, 0.007170, -0.002716, -0.001046, 0.000898, 0.000000 },
	{ 0.000754, 0.001195, -0.004727, 0.000123, 0.015807, -0.015658, -0.040379, 0.284391, -0.019348, 0.040562, -0.007732, -0.009988, 0.005367, 0.001296, -0.001570, 0.000135, 0.000000 }
}};

#endif

int main(int argc, char **argvp)
{
    util::sine_source<float> sigsrc { rm_math::hz_to_rps(F, Fs) };
    auto sig = util::make_aligned_ptr<float>(SIG_SAMPLE_SIZE);
    sigsrc.get(sig);
    util::aligned_ptr<float> out{ };

    FILE *f = fopen("test-resampler-sig.txt", "w");
    util::printReal(f, sig.size(), sig.data());
    fclose(f);

    //// Interpolation test
    dsp::rational_resampler_ff pinterp { 4, 1, lp_poly_hamming_8p5K_48k, 4};

    f = fopen("test-resampler-interp.txt", "w");

    pinterp.resample(sig, out);
    util::printReal(f, out.size(), out.data());
    fclose(f);

    //// Decimation test
    dsp::rational_resampler_ff pdecim { 1, 4, lp_poly_hamming_8p5K_48k};

    f = fopen("test-resampler-decim.txt", "w");

    for (int i=0;i < 3;i++)
    {
        pdecim.resample(sig, out);
        util::printReal(f, out.size(), out.data());
        util::init_aligned_ptr_on_resize(sig, sig.size() + i);
        sigsrc.get(sig);
    }

    fclose(f);

    //// Interpolation -> decimation test
    dsp::rational_resampler_ff resample { 4, 5, lp_poly_hamming_8p5K_48k, 4};

    f = fopen("test-resampler-resample.txt", "w");

    for (int i=0;i < 3;i++)
    {
        resample.resample(sig, out);
        util::printReal(f, out.size(), out.data());
        util::init_aligned_ptr_on_resize(sig, sig.size() + i);
        sigsrc.get(sig);
    }

    fclose(f);

    return 0;
}
