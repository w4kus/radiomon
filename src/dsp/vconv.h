#pragma once

#include <vector>
#include <complex>

namespace dsp {

std::vector<float> vconvolve(const float *b, const size_t n, const float *h, const size_t m);
std::vector<std::complex<float>> vconvolve(const std::complex<float> *b, const size_t n, const float *h, const size_t m);

}
