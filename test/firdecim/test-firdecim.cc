// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdio.h>
#include <complex>
#include <vector>
#include <cstring>

#include "firdecim.h"
#include "cmdline.h"
#include "aligned_ptr.h"
#include "cmemset.h"

// 8K @ 48Ksps
const float test_sig[256] =
{
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000,
	0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000,
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000,
	0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000,
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000,
	0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000,
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000,
	0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000,
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000,
	0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000,
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000,
	0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000,
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000,
	0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000,
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000,
	0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000,
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000,
	0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000,
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000,
	0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000,
	0.500000, -0.500000, -1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000,
	-1.000000, -0.500000, 0.500000, 1.000000, 0.500000, -0.500000, -1.000000, -0.500000
};

const float lp_hamming_8p5K[65] =
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

constexpr size_t tapNum = sizeof(lp_hamming_8p5K) / sizeof(lp_hamming_8p5K[0]);
constexpr size_t sigNum = sizeof(test_sig) / sizeof(test_sig[0]);
constexpr size_t chunkSize = 64;
constexpr size_t chunkNum = sigNum / chunkSize;
constexpr size_t M = 3;

int main(int argc, char **argvp)
{
    FILE *f = fopen("firdecim-float.txt", "w");

#if 0
	dsp::firdecim_ff decim { M, util::make_aligned_ptr<float>(tapNum, lp_hamming_8p5K) };
	auto sig = util::make_aligned_ptr<float>(chunkSize);
	auto out = util::aligned_ptr<float>{ };

    for (size_t i=0;i < chunkNum;i++)
    {
		memcpy(&sig[0], &test_sig[i * chunkSize], chunkSize * sizeof(float));
		decim.decim(sig, out);
		util::printReal(f, out.size(), out.get());
    }
#else
	dsp::firdecim_ff decim { M, util::make_aligned_ptr<float>(tapNum, lp_hamming_8p5K) };
	size_t idx = 0;
	auto sig = util::make_aligned_ptr<float>(chunkSize, test_sig);
	auto out = util::aligned_ptr<float>{ };
	decim.decim(sig, out);
	util::printReal(f, out.size(), out.get());
	idx += chunkSize;

	sig = util::make_aligned_ptr<float>(chunkSize / 2, &test_sig[idx]);
	decim.decim(sig, out);
	util::printReal(f, out.size(), out.get());
	idx += chunkSize / 2;

	sig = util::make_aligned_ptr<float>(chunkSize - 12, &test_sig[idx]);
	decim.decim(sig, out);
	util::printReal(f, out.size(), out.get());
	idx += chunkSize - 12;

	sig = util::make_aligned_ptr<float>(100, &test_sig[idx]);
	decim.decim(sig, out);
	util::printReal(f, out.size(), out.get());
#endif

    fclose(f);

    f = fopen("firdecim-complex.txt", "w");

    dsp::firdecim_cc decimc { M, util::make_aligned_ptr<float>(tapNum, lp_hamming_8p5K) };
	util::aligned_ptr<util::complex_f> cout { };
	auto csig = util::make_aligned_ptr<util::complex_f>(sigNum);

	for (size_t i=0;i < sigNum;i++)
		csig[i] = { test_sig[i], 0.0f };

	auto cin = util::make_aligned_ptr<util::complex_f>(chunkSize);

    for (size_t i=0;i < chunkNum;i++)
    {
		std::memcpy(&cin[0], &csig[i * chunkSize], chunkSize * sizeof(util::complex_f));
		decimc.decim(cin, cout);
        util::printComplex(f, cout.size(), cout.get());
    }

	fclose(f);

    return 0;
}
