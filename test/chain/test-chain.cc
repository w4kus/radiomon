// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdlib.h>

#include "chain.h"
#include "timer.h"
#include "cmdline.h"

// Blocks for testing
#include "vector-source.h"
#include "hilbert.h"
#include "complex-float.h"
#include "callback.h"
#include "signal-source.h"
#include "rational-resampler.h"
/*
const float lp_blackman_1p5k_48k[91] =
{
	-0.000000, 0.000003, 0.000012, 0.000029, 0.000056, 0.000091, 0.000130, 0.000171, 0.000204, 0.000223, 0.000215, 0.000168,
	0.000071, -0.000091, -0.000326, -0.000644, -0.001045, -0.001526, -0.002073, -0.002664, -0.003266, -0.003834, -0.004314, -0.004642,
	-0.004747, -0.004555, -0.003992, -0.002988, -0.001480, 0.000576, 0.003211, 0.006429, 0.010212, 0.014511, 0.019251, 0.024331,
	0.029624, 0.034984, 0.040253, 0.045262, 0.049845, 0.053845, 0.057119, 0.059548, 0.061042, 0.061547, 0.061042, 0.059548,
	0.057119, 0.053845, 0.049845, 0.045262, 0.040253, 0.034984, 0.029624, 0.024331, 0.019251, 0.014511, 0.010212, 0.006429,
	0.003211, 0.000576, -0.001480, -0.002988, -0.003992, -0.004555, -0.004747, -0.004642, -0.004314, -0.003834, -0.003266, -0.002664,
	-0.002073, -0.001526, -0.001045, -0.000644, -0.000326, -0.000091, 0.000071, 0.000168, 0.000215, 0.000223, 0.000204, 0.000171,
	0.000130, 0.000091, 0.000056, 0.000029, 0.000012, 0.000003, -0.000000
};
*/

const std::array<std::array<float, 23>, 4> lp_blackman_1p5k_48k_poly =
{{
	{ -0.000000, 0.000056, 0.000204, 0.000071, -0.001045, -0.003266, -0.004747, -0.001480, 0.010212, 0.029624, 0.049845, 0.061042, 0.057119, 0.040253, 0.019251, 0.003211, -0.003992, -0.004314, -0.002073, -0.000326, 0.000215, 0.000130, 0.000012 },
	{ 0.000003, 0.000091, 0.000223, -0.000091, -0.001526, -0.003834, -0.004555, 0.000576, 0.014511, 0.034984, 0.053845, 0.061547, 0.053845, 0.034984, 0.014511, 0.000576, -0.004555, -0.003834, -0.001526, -0.000091, 0.000223, 0.000091, 0.000003 },
	{ 0.000012, 0.000130, 0.000215, -0.000326, -0.002073, -0.004314, -0.003992, 0.003211, 0.019251, 0.040253, 0.057119, 0.061042, 0.049845, 0.029624, 0.010212, -0.001480, -0.004747, -0.003266, -0.001045, 0.000071, 0.000204, 0.000056, -0.000000 },
	{ 0.000029, 0.000171, 0.000168, -0.000644, -0.002664, -0.004642, -0.002988, 0.006429, 0.024331, 0.045262, 0.059548, 0.059548, 0.045262, 0.024331, 0.006429, -0.002988, -0.004642, -0.002664, -0.000644, 0.000168, 0.000171, 0.000029, 0.000000 }
}};

constexpr dsp::rate_t Fs = 48000;
constexpr dsp::rate_t F = 800;
constexpr size_t sampleNum = Fs / F * 48;
constexpr int M = 4;

static FILE *f;
void chainCallback(const util::aligned_ptr<float> &buff)
{
    // printf("Rcv %.1f %.1f %.1f\n", data[0], data[1], data[2]);
    util::printReal(f, buff.size(), buff.data());
}

int main(int argc, char **argvp)
{
    util::chain theChain("TEST_CHAIN");

#if 0
    theChain.add(std::make_shared<dsp::endpoints::vector_source_ff>(std::vector<float>{ 1.0f, 2.0f, 3.0f }, 4800), "VECTOR_SOURCE");
    theChain.add(std::make_shared<dsp::hilbert>(), "HILBERT");
    theChain.add(std::make_shared<dsp::complex_float>(), "COMPLEX_FLOAT");
    theChain.add(std::make_shared<dsp::endpoints::callback_ff>(chainCallback), "CALLBACK");
#else
    f = fopen("test-signal-source.txt", "w");

    theChain.add(std::make_shared<dsp::endpoints::signal_source_ff>(sampleNum, F, Fs), "SIG_SOURCE");
    theChain.add(std::make_shared<dsp::rational_resampler_ff>(1, M, lp_blackman_1p5k_48k_poly), "RESAMPLER");
    theChain.add(std::make_shared<dsp::endpoints::callback_ff>(chainCallback), "CALLBACK");
#endif

    if (!theChain.check())
    {
        printf("Chain check failed\n");
        return -1;
    }

#if 0
    auto tmr = util::timer::StartTimer();

    for (int i=0;i < 100000;i++)
    {
        printf("iter %d\n", i);
        theChain.iterate();
    }

    printf("-- %u\n", util::timer::EndTimer(tmr));
#else
    theChain.iterate();
    fclose(f);
#endif

    return 0;
}
