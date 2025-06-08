#include <memory>
#include <cmath>
#include <stdio.h>
#include <vector>
#include <complex>
#include <string>
#include "vconv.h"
#include "conv.h"
#include "conv_full.h"

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

static void testDirectReal(void);
static void testDirectComplex(void);
static void printComplex(FILE *f, std::complex<float> val);

int main(int argc, char **argvp)
{
    testDirectReal();
    testDirectComplex();

    return 0;
}

static void testDirectReal(void)
{
    auto sig = std::make_unique<float[]>(256);
    FILE *fsig = fopen("fsig.txt", "w");
    FILE *fconv = fopen("ffilt.txt", "w");

    if (fsig && fconv)
    {
        for (int i=0;i < 256;i++)
        {
            sig[i] = std::cos(2*M_PI*2*i/48);
            fprintf(fsig, "%0.4f ", sig[i]);
        }
    }
    else
    {
        fprintf(stderr, "Can't open output files\n");
        if (fsig) fclose(fsig);
        if (fconv) fclose(fconv);
        return;
    }
    
    fclose(fsig);

    auto vconv = dsp::vconvolve(sig.get(), 256, lp_test_6k_48k, 65);
    // auto vconv = dsp::conv<float>().convolve(sig.get(), 256, lp_test_6k_48k, 65);
    // auto vconv = dsp::conv_full<float>().convolve(sig.get(), 256, lp_test_6k_48k, 65);

    for (int i=0;i < (int)vconv.size();i++)
        fprintf(fconv, "%0.4f ", vconv[i]);

    fclose(fconv);
}

static void testDirectComplex(void)
{
    auto sig = std::make_unique<std::complex<float>[]>(256);
    FILE *fsig = fopen("cfsig.txt", "w");
    FILE *fconv = fopen("cffilt.txt", "w");

    if (fsig && fconv)
    {
        for (int i=0;i < 256;i++)
        {
            sig[i].real(std::cos(2*M_PI*3.5*i/48));
            sig[i].imag(std::sin(2*M_PI*3.5*i/48));
            printComplex(fsig, sig[i]);
        }
    }
    else
    {
        fprintf(stderr, "Can't open output files\n");
        if (fsig) fclose(fsig);
        if (fconv) fclose(fconv);
        return;
    }
    
    fclose(fsig);

    auto vconv = dsp::vconvolve(sig.get(), 256, lp_test_6k_48k, 65);
    // auto vconv = dsp::conv<std::complex<float>>().convolve(sig.get(), 256, lp_test_6k_48k_c, 65);
    // auto vconv = dsp::conv_full<std::complex<float>>().convolve(sig.get(), 256, lp_test_6k_48k_c, 65);

    for (int i=0;i < (int)vconv.size();i++)
        printComplex(fconv, vconv[i]);

    fclose(fconv);
}

static void printComplex(FILE *f, std::complex<float> val)
{
    // v = cell2mat(textscan(f, "%f"))'
    fprintf(f, "%f", val.real());

    if (val.imag() >= 0)
        fprintf(f, "+");

    fprintf(f, "%0.4fj ", val.imag());
}