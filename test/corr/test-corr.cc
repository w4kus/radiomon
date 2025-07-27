// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdio.h>
#include <complex>
#include <vector>
#include <memory>

#include "corr.h"
#include "cmdline.h"
#include "peak.h"

const float test_corr_seq_f[] =
{
    // DMR BS source sync: voice (compliment: data)
    //   7      5      5      F     D      7      D      F      7      5      F       7
    // +3 -3 +3 +3  +3 +3  -3 -3  -3 +3  +3 -3  -3 +3  -3 -3  +3 -3  +3 +3  -3 -3  +3 -3
    //
    +1, -1, +1, +1,     // 75
    +1, +1, -1, -1,     // 5F
    -1, +1, +1, -1,     // D7
    -1, +1, -1, -1,     // DF
    +1, -1, +1, +1,     // 75
    -1, -1, +1, -1,     // F7
};

const std::complex<float> test_corr_seq_c[] =
{
    // DMR BS source sync: voice (compliment: data)
    //   7      5      5      F     D      7      D      F      7      5      F       7
    // +3 -3 +3 +3  +3 +3  -3 -3  -3 +3  +3 -3  -3 +3  -3 -3  +3 -3  +3 +3  -3 -3  +3 -3
    //
    +1, -1, +1, +1,     // 75
    +1, +1, -1, -1,     // 5F
    -1, +1, +1, -1,     // D7
    -1, +1, -1, -1,     // DF
    +1, -1, +1, +1,     // 75
    -1, -1, +1, -1,     // F7
};

const float test_symbol_seq_f[] =
{
    // 24 various DMR symbols
    (float)+1/3, +1, -1, (float)-1/3,
    (float)-1/3, (float)+1/3, +1, +1,
    +1, -1, (float)-1/3, (float)+1/3,
    +1, +1, +1, -1,
    -1, (float)+1/3, (float)-1/3, (float)-1/3,
    (float)-1/3, +1, -1, (float)+1/3,

    // DMR BS source sync: voice (compliment: data)
    //   7      5      5      F     D      7      D      F      7      5      F       7
    // +3 -3 +3 +3  +3 +3  -3 -3  -3 +3  +3 -3  -3 +3  -3 -3  +3 -3  +3 +3  -3 -3  +3 -3
    //
    +1, -1, +1, +1,     // 75
    +1, +1, -1, -1,     // 5F
    -1, +1, +1, -1,     // D7
    -1, +1, -1, -1,     // DF
    +1, -1, +1, +1,     // 75
    -1, -1, +1, -1,     // F7

    // 24 various DMR symbols
    +1, +1, -1, -1,
    +1, +1, -1, (float)+1/3,
    (float)+1/3, (float)-1/3, (float)+1/3, (float)-1/3,
    -1, -1, +1, -1,
    +1, (float)+1/3, (float)-1/3, -1,
    -1, -1, +1, (float)-1/3
};

const std::complex<float> test_symbol_seq_c[] =
{
    // 24 various DMR symbols
    (float)+1/3, +1, -1, (float)-1/3,
    (float)-1/3, (float)+1/3, +1, +1,
    +1, -1, (float)-1/3, (float)+1/3,
    +1, +1, +1, -1,
    -1, (float)+1/3, (float)-1/3, (float)-1/3,
    (float)-1/3, +1, -1, (float)+1/3,

    // DMR BS source sync: voice (compliment: data)
    //   7      5      5      F     D      7      D      F      7      5      F       7
    // +3 -3 +3 +3  +3 +3  -3 -3  -3 +3  +3 -3  -3 +3  -3 -3  +3 -3  +3 +3  -3 -3  +3 -3
    //
    +1, -1, +1, +1,     // 75
    +1, +1, -1, -1,     // 5F
    -1, +1, +1, -1,     // D7
    -1, +1, -1, -1,     // DF
    +1, -1, +1, +1,     // 75
    -1, -1, +1, -1,     // F7

    // 24 various DMR symbols
    +1, +1, -1, -1,
    +1, +1, -1, (float)+1/3,
    (float)+1/3, (float)-1/3, (float)+1/3, (float)-1/3,
    -1, -1, +1, -1,
    +1, (float)+1/3, (float)-1/3, -1,
    -1, -1, +1, (float)-1/3
};

constexpr size_t TEST_SYMBOL_SEQ_SIZE = sizeof(test_symbol_seq_f) / sizeof(test_symbol_seq_f[0]);
constexpr size_t TEST_CORR_SEQ_SIZE = sizeof(test_corr_seq_f) / sizeof(test_corr_seq_f[0]);

int main(int argc, char **argvp)
{
    FILE *fcorr = fopen("corr-float.txt", "w");
    auto corr = std::make_unique<dsp::corr>(TEST_CORR_SEQ_SIZE, dsp::corr::buffer_type_real);
    auto peak = util::peak<float>();

    // set buffer X to the sequence with which we want to correlate 
    corr->setX(test_corr_seq_f);

    for (size_t i=0;i < TEST_SYMBOL_SEQ_SIZE;i++)
    {
        auto coeff = corr->addY(test_symbol_seq_f[i]);
        float pk = 0.0f;

        fprintf(fcorr, "%.4f\n", coeff);

        if (peak.get(coeff, pk))
            fprintf(fcorr, "*** PEAK %.4f\n", pk);
    }

    fclose(fcorr);

    corr = std::make_unique<dsp::corr>(TEST_CORR_SEQ_SIZE, dsp::corr::buffer_type_complex);

    fcorr = fopen("corr-complex.txt", "w");

    std::vector<std::complex<float>> vals;
    vals.reserve(TEST_SYMBOL_SEQ_SIZE);

    // set buffer X to the sequence with which we want to correlate 
    corr->setX(test_corr_seq_c);

    std::complex<float> number;

    for (size_t i=0;i < TEST_SYMBOL_SEQ_SIZE;i++)
        vals.push_back(corr->addY(std::conj(test_symbol_seq_c[i])));

    util::printComplex(fcorr, vals);

    fclose(fcorr);

    return 0;
}
