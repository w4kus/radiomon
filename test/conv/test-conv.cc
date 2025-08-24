// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <memory>
#include <cmath>
#include <stdio.h>
#include <vector>
#include <complex>
#include <string>
#include "conv.h"
#include "conv_full.h"
#include "fconv.h"
#include "cmdline.h"

const float lp_test_6k_48k[65] = 
{
	-0.0001, -0.0004, -0.0007, -0.0010, -0.0013, -0.0014, -0.0013, -0.0007, 0.0002, 0.0015, 0.0031, 0.0047, 
	0.0058, 0.0061, 0.0053, 0.0031, -0.0005, -0.0053, -0.0106, -0.0155, -0.0190, -0.0200, -0.0174, -0.0105, 
	0.0008, 0.0164, 0.0353, 0.0561, 0.0770, 0.0960, 0.1111, 0.1210, 0.1243, 0.1210, 0.1111, 0.0960, 
	0.0770, 0.0561, 0.0353, 0.0164, 0.0008, -0.0105, -0.0174, -0.0200, -0.0190, -0.0155, -0.0106, -0.0053, 
	-0.0005, 0.0031, 0.0053, 0.0061, 0.0058, 0.0047, 0.0031, 0.0015, 0.0002, -0.0007, -0.0013, -0.0014, 
	-0.0013, -0.0010, -0.0007, -0.0004, -0.0001
};

const std::complex<float> lp_test_6k_48k_c[65] = 
{
	{-0.0001, 0}, {-0.0004, 0}, {-0.0007, 0}, {-0.0010, 0}, {-0.0013, 0}, {-0.0014, 0}, {-0.0013, 0}, {-0.0007, 0}, 
	{0.0002, 0}, {0.0015, 0}, {0.0031, 0}, {0.0047, 0}, {0.0058, 0}, {0.0061, 0}, {0.0053, 0}, {0.0031, 0}, 
	{-0.0005, 0}, {-0.0053, 0}, {-0.0106, 0}, {-0.0155, 0}, {-0.0190, 0}, {-0.0200, 0}, {-0.0174, 0}, {-0.0105, 0}, 
	{0.0008, 0}, {0.0164, 0}, {0.0353, 0}, {0.0561, 0}, {0.0770, 0}, {0.0960, 0}, {0.1111, 0}, {0.1210, 0}, 
	{0.1243, 0}, {0.1210, 0}, {0.1111, 0}, {0.0960, 0}, {0.0770, 0}, {0.0561, 0}, {0.0353, 0}, {0.0164, 0}, 
	{0.0008, 0}, {-0.0105, 0}, {-0.0174, 0}, {-0.0200, 0}, {-0.0190, 0}, {-0.0155, 0}, {-0.0106, 0}, {-0.0053, 0}, 
	{-0.0005, 0}, {0.0031, 0}, {0.0053, 0}, {0.0061, 0}, {0.0058, 0}, {0.0047, 0}, {0.0031, 0}, {0.0015, 0}, 
	{0.0002, 0}, {-0.0007, 0}, {-0.0013, 0}, {-0.0014, 0}, {-0.0013, 0}, {-0.0010, 0}, {-0.0007, 0}, {-0.0004, 0}, 
	{-0.0001, 0}
};

constexpr int TEST_COEFF_SIZE   (65);
constexpr int TEST_SIG_SIZE     (256);

static std::vector<float> rsig;
static std::vector<std::complex<float>> csig;

static void testDirectReal(void);
static void testDirectComplex(void);
static void testDftReal(void);
static void testDftComplex(void);

int main(int argc, char **argvp)
{
    FILE *frsig = fopen("rsig.txt", "w");
    FILE *fcsig = fopen("csig.txt", "w");

    rsig.reserve(TEST_SIG_SIZE);
    for (int i=0;i < TEST_SIG_SIZE;i++)
        rsig.push_back(std::cos(2*M_PI*3.5*i/48));

    util::printReal(frsig, rsig);

    csig.reserve(TEST_SIG_SIZE);
    for (int i=0;i < TEST_SIG_SIZE;i++)
        csig.push_back({(float)std::cos(2*M_PI*3.5*i/48), (float)std::sin(2*M_PI*3.5*i/48)});

    util::printComplex(fcsig, csig);

    fclose(frsig);
    fclose(fcsig);

    testDirectReal();
    testDirectComplex();
    testDftReal();
    testDftComplex();

    return 0;
}

static void testDirectReal(void)
{
    auto f = fopen("dsp_conv.txt", "w");
    auto conv = util::convolve<float>(rsig.data(), rsig.size(), lp_test_6k_48k, TEST_COEFF_SIZE);
    util::printReal(f, conv);
    fclose(f);

    f = fopen("dsp_conv_full.txt", "w");
    conv = util::fullconvolve<float>(rsig.data(), rsig.size(), lp_test_6k_48k, TEST_COEFF_SIZE);
    util::printReal(f, conv);
    fclose(f);
}

static void testDirectComplex(void)
{
    auto f = fopen("dsp_conv_c.txt", "w");
    auto conv = util::convolve<std::complex<float>>(csig.data(), csig.size(), lp_test_6k_48k_c, TEST_COEFF_SIZE);
    util::printComplex(f, conv);
    fclose(f);

    f = fopen("dsp_conv_full_c.txt", "w");
    conv = util::convolve<std::complex<float>>(csig.data(), csig.size(), lp_test_6k_48k_c, TEST_COEFF_SIZE);
    util::printComplex(f, conv);
    fclose(f);
}

static void testDftReal(void)
{
    FILE *f = fopen("dsp_fconv.txt", "w");
    auto dftconv = util::fconv{ lp_test_6k_48k, TEST_COEFF_SIZE, TEST_SIG_SIZE };
    auto conv = dftconv.convolve(rsig.data());
    util::printReal(f, conv);
    fclose(f);
}

static void testDftComplex(void)
{
    FILE *f = fopen("dsp_fconv_c.txt", "w");
    auto dftconv = util::fconv{ lp_test_6k_48k_c, TEST_COEFF_SIZE, TEST_SIG_SIZE };
    auto conv = dftconv.convolve(csig.data());
    util::printComplex(f, conv);
    fclose(f);
}
