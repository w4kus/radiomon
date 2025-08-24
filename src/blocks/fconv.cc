// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
// 
// Licensed under the MIT License - see LICENSE file for details.

//! \file fconv.cc

#include <cstring>
#include <fconv.h>

using namespace util;

fconv::fconv(const float *h, const size_t n, const size_t m) :
    m_FwdPlan(nullptr),
    m_RevPlan(nullptr),
    m_N(n),
    m_M(m),
    m_DftSize(m+n-1),
    m_Taps(nullptr),
    m_Buff(nullptr),
    m_A(1.0f/(float)m_DftSize)
{
    m_Taps = (fftwf_complex *)fftwf_malloc(m_DftSize * sizeof(fftwf_complex));
    std::memset(m_Taps, 0, m_DftSize * sizeof(fftwf_complex));

    for (size_t i=0;i < n;i++)
    {
        m_Taps[i][0] = h[i];
        m_Taps[i][1] = 0.0f;
    }

    setupTaps();
}

fconv::fconv(const std::complex<float> *h, const size_t n, const size_t m) :
    m_FwdPlan(nullptr),
    m_RevPlan(nullptr),
    m_N(n),
    m_M(m),
    m_DftSize(m+n-1),
    m_Taps(nullptr),
    m_Buff(nullptr),
    m_A(1.0f/(float)m_DftSize)
{
    m_Taps = (fftwf_complex *)fftwf_malloc(m_DftSize * sizeof(fftwf_complex));
    std::memset(m_Taps, 0, m_DftSize * sizeof(fftwf_complex));

    for (size_t i=0;i < n;i++)
    {
        m_Taps[i][0] = h[i].real();
        m_Taps[i][1] = h[i].imag();
    }

    setupTaps();
}


fconv::~fconv()
{
    fftwf_free(m_Taps);
    fftwf_free(m_Buff);
    fftwf_destroy_plan(m_FwdPlan);
    fftwf_destroy_plan(m_RevPlan);
}

void fconv::setupTaps()
{
    m_Buff = (fftwf_complex *)fftwf_malloc(m_DftSize * sizeof(fftwf_complex));

    fftwf_plan tapPlan = fftwf_plan_dft_1d(m_DftSize, m_Taps, m_Taps, FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(tapPlan);
    fftwf_destroy_plan(tapPlan);

    m_FwdPlan = fftwf_plan_dft_1d(m_DftSize, m_Buff, m_Buff, FFTW_FORWARD, FFTW_ESTIMATE);
    m_RevPlan = fftwf_plan_dft_1d(m_DftSize, m_Buff, m_Buff, FFTW_BACKWARD, FFTW_ESTIMATE);
}

std::vector<float> fconv::convolve(const float *b, bool stripEdges)
{
    std::vector<float> res;

    std::memset(m_Buff, 0, m_DftSize * sizeof(fftwf_complex));

    for (size_t i=0;i < m_M;i++)
    {
        m_Buff[i][0] = b[i];
        m_Buff[i][1] = 0.0f;
    }

    execute();

    size_t start = (stripEdges) ? m_N : 0;
    size_t end = (stripEdges) ? m_DftSize - m_N + 1: m_DftSize - 1;

    for (size_t i=start;i <= end;i++)
    {
        float val = m_Buff[i][0] * m_A;
        res.push_back(val);
    }

    return res;
}

std::vector<std::complex<float>> fconv::convolve(const std::complex<float> *b, bool stripEdges)
{
    std::vector<std::complex<float>> res;

    std::memset(m_Buff, 0, m_DftSize * sizeof(fftwf_complex));

    for (size_t i=0;i < m_M;i++)
    {
        m_Buff[i][0] = b[i].real();
        m_Buff[i][1] = b[i].imag();
    }

    execute();
    
    size_t start = (stripEdges) ? m_N : 0;
    size_t end = (stripEdges) ? m_DftSize - m_N + 1: m_DftSize - 1;

    for (size_t i=start;i <= end;i++)
    {
        std::complex<float> val(m_Buff[i][0] * m_A, m_Buff[i][1] * m_A);
        res.push_back(val);
    }

    return res;
}

void fconv::execute()
{
    fftwf_execute(m_FwdPlan);

    for (size_t i=0;i < m_DftSize;i++)
    {
        float resig = m_Buff[i][0];
        float imsig = m_Buff[i][1];
        float retap = m_Taps[i][0];
        float imtap = m_Taps[i][1];
        
        m_Buff[i][0] = (resig * retap) - (imsig * imtap);
        m_Buff[i][1] = (imsig * retap) + (resig * imtap);
    }

    fftwf_execute(m_RevPlan);
}
