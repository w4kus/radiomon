#pragma once

#include "stdint.h"
#include <vector>
#include <complex>

namespace dsp {

std::vector<float> vconvolve(const float *b, const uint32_t n, const float *h, const uint32_t m);
std::vector<std::complex<float>> vconvolve(const std::complex<float> *b, const uint32_t n, const float *h, const uint32_t m);

}
