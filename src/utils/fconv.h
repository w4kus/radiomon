// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <vector>
#include <complex>
#include <fftw3.h>

namespace util {

/*! \brief Frequency domain convolution.
 *
 * Implements convolution in the frequency domain using the [FFTW library](https://www.fftw.org).
 *
*/

class fconv
{
public:
    fconv() = delete;
    fconv(const fconv &) = delete;
    fconv& operator=(const fconv&) = delete;

    ~fconv();

    //! Create an instance with real (*float*) taps.
    //! @param [in] h Real taps.
    //! @param [in] n The number of taps.
    //! @param [in] m The number of samples per call for this instance.
    fconv(const float *h, const size_t n, const size_t m);

    //! Create an instance with complex taps.
    //! @param [in] h Complex taps.
    //! @param [in] n The number of taps.
    //! @param [in] m The number of samples per call for this instance.
    fconv(const std::complex<float> *h, size_t n, const size_t m);

    //! @param [in] b           The samples to convolve with the taps of this instance.
    //! @param [in] stripEdges  If true, strip the overlapping edges, otherwise return
    //!             the full convolution.
    //! @return     If **stripEdges** is true, returns the m - n + 1 results in a vector of
    //!             type *float*. Otherwise, returns m + n - 1 results.
    std::vector<float> convolve(const float *b, bool stripEdges = false);

    //! @param [in] b           The samples to convolve with the taps of this instance.
    //! @param [in] stripEdges  If true, strip the overlapping edges, otherwise return
    //!             the full convolution.
    //! @return     If **stripEdges** is true, returns the m - n + 1 results in a vector of
    //!             type *complex*. Otherwise, returns m + n - 1 results.
    std::vector<std::complex<float>> convolve(const std::complex<float> *b, bool stripEdges = false);

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
