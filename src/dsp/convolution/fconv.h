// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <vector>
#include <complex>
#include <fftw3.h>

namespace dsp {

class fconv
{
public:
    fconv() = delete;
    fconv(const fconv &) = delete;
    fconv& operator=(const fconv&) = delete;

    fconv(const float *h, const size_t n, const size_t m);
    fconv(const std::complex<float> *h, size_t n, const size_t m);

    virtual ~fconv();

    std::vector<float> convolve(const float *b, bool stripEdges = true);
    std::vector<std::complex<float>> convolve(const std::complex<float> *b, bool stripEdges = true);

private:
    fftwf_plan m_FwdPlan;
    fftwf_plan m_RevPlan;

    size_t m_N;
    size_t m_M;
    size_t m_DftSize;
    fftwf_complex *m_Taps;
    fftwf_complex *m_Buff;

    float m_A;

    void setupTaps();
    void execute();
};

}
