/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "grfirfilt_impl.h"
#include <gnuradio/io_signature.h>
#include "../radiomon/utils/aligned_ptr.h"


namespace gr {
namespace radiomon {

const float lp_hamming_6k_large[321] =
{
	-0.000078, -0.000160, -0.000149, -0.000051, 0.000080, 0.000167, 0.000159, 0.000057,
    -0.000084, -0.000181, -0.000175, -0.000065, 0.000091, 0.000201, 0.000198, 0.000076,
    -0.000100, -0.000228, -0.000228, -0.000090, 0.000112, 0.000262, 0.000265, 0.000108,
    -0.000127, -0.000304, -0.000311, -0.000130, 0.000144, 0.000353, 0.000365, 0.000157,
    -0.000164, -0.000411, -0.000429, -0.000188, 0.000186, 0.000477, 0.000503, 0.000226,
    -0.000210, -0.000553, -0.000588, -0.000270, 0.000236, 0.000637, 0.000684, 0.000320,
    -0.000263, -0.000731, -0.000792, -0.000379, 0.000293, 0.000836, 0.000913, 0.000446,
    -0.000324, -0.000951, -0.001049, -0.000522, 0.000356, 0.001078, 0.001200, 0.000608,
    -0.000389, -0.001217, -0.001367, -0.000705, 0.000423, 0.001369, 0.001552, 0.000815,
    -0.000457, -0.001535, -0.001757, -0.000939, 0.000492, 0.001716, 0.001984, 0.001079,
    -0.000528, -0.001913, -0.002235, -0.001236, 0.000563, 0.002130, 0.002512, 0.001412,
    -0.000598, -0.002367, -0.002821, -0.001612, 0.000632, 0.002627, 0.003164, 0.001837,
    -0.000666, -0.002915, -0.003548, -0.002093, 0.000700, 0.003235, 0.003980, 0.002385,
    -0.000732, -0.003592, -0.004469, -0.002720, 0.000762, 0.003995, 0.005027, 0.003108,
    -0.000792, -0.004455, -0.005672, -0.003561, 0.000819, 0.004986, 0.006426, 0.004099,
    -0.000845, -0.005610, -0.007323, -0.004745, 0.000869, 0.006358, 0.008411, 0.005540,
    -0.000891, -0.007277, -0.009764, -0.006541, 0.000911, 0.008443, 0.011505, 0.007848,
    -0.000928, -0.009982, -0.013842, -0.009631, 0.000943, 0.012132, 0.017171, 0.012224,
    -0.000955, -0.015381, -0.022340, -0.016373, 0.000965, 0.020928, 0.031553, 0.024148,
    -0.000971, -0.032711, -0.052899, -0.044243, 0.000976, 0.075687, 0.159165, 0.224464,
    0.249130, 0.224464, 0.159165, 0.075687, 0.000976, -0.044243, -0.052899, -0.032711,
    -0.000971, 0.024148, 0.031553, 0.020928, 0.000965, -0.016373, -0.022340, -0.015381,
    -0.000955, 0.012224, 0.017171, 0.012132, 0.000943, -0.009631, -0.013842, -0.009982,
    -0.000928, 0.007848, 0.011505, 0.008443, 0.000911, -0.006541, -0.009764, -0.007277,
    -0.000891, 0.005540, 0.008411, 0.006358, 0.000869, -0.004745, -0.007323, -0.005610,
    -0.000845, 0.004099, 0.006426, 0.004986, 0.000819, -0.003561, -0.005672, -0.004455,
    -0.000792, 0.003108, 0.005027, 0.003995, 0.000762, -0.002720, -0.004469, -0.003592,
    -0.000732, 0.002385, 0.003980, 0.003235, 0.000700, -0.002093, -0.003548, -0.002915,
    -0.000666, 0.001837, 0.003164, 0.002627, 0.000632, -0.001612, -0.002821, -0.002367,
    -0.000598, 0.001412, 0.002512, 0.002130, 0.000563, -0.001236, -0.002235, -0.001913,
    -0.000528, 0.001079, 0.001984, 0.001716, 0.000492, -0.000939, -0.001757, -0.001535,
    -0.000457, 0.000815, 0.001552, 0.001369, 0.000423, -0.000705, -0.001367, -0.001217,
    -0.000389, 0.000608, 0.001200, 0.001078, 0.000356, -0.000522, -0.001049, -0.000951,
    -0.000324, 0.000446, 0.000913, 0.000836, 0.000293, -0.000379, -0.000792, -0.000731,
    -0.000263, 0.000320, 0.000684, 0.000637, 0.000236, -0.000270, -0.000588, -0.000553,
    -0.000210, 0.000226, 0.000503, 0.000477, 0.000186, -0.000188, -0.000429, -0.000411,
    -0.000164, 0.000157, 0.000365, 0.000353, 0.000144, -0.000130, -0.000311, -0.000304,
    -0.000127, 0.000108, 0.000265, 0.000262, 0.000112, -0.000090, -0.000228, -0.000228,
    -0.000100, 0.000076, 0.000198, 0.000201, 0.000091, -0.000065, -0.000175, -0.000181,
    -0.000084, 0.000057, 0.000159, 0.000167, 0.000080, -0.000051, -0.000149, -0.000160,
    -0.000078
};

constexpr size_t tapNumLarge = sizeof(lp_hamming_6k_large) / sizeof(lp_hamming_6k_large[0]);

template<typename T>
typename grfirfilt<T>::sptr grfirfilt<T>::make()
{ 
    return gnuradio::make_block_sptr<grfirfilt_impl<T>>();
}


/*
 * The private constructor
 */
template<typename T>
grfirfilt_impl<T>::grfirfilt_impl()
    : gr::sync_block("grfirfilt",
                     gr::io_signature::make(
                         1 /* min inputs */, 1 /* max inputs */, sizeof(T)),
                     gr::io_signature::make(
                         1 /* min outputs */, 1 /*max outputs */, sizeof(T)))
{
}

template<>
grfirfilt_impl<float>::grfirfilt_impl()
    : gr::sync_block("grfirfilt",
                     gr::io_signature::make(
                         1 /* min inputs */, 1 /* max inputs */, sizeof(float)),
                     gr::io_signature::make(
                         1 /* min outputs */, 1 /*max outputs */, sizeof(float)))
{
    d_FilterF = std::make_unique<dsp::firfilter_ff>(util::make_aligned_ptr<float>(tapNumLarge, lp_hamming_6k_large));
}

template<>
grfirfilt_impl<gr_complex>::grfirfilt_impl()
    : gr::sync_block("grfirfilt",
                     gr::io_signature::make(
                         1 /* min inputs */, 1 /* max inputs */, sizeof(gr_complex)),
                     gr::io_signature::make(
                         1 /* min outputs */, 1 /*max outputs */, sizeof(gr_complex)))
{
    d_FilterC = std::make_unique<dsp::firfilter_cc>(util::make_aligned_ptr<float>(tapNumLarge, lp_hamming_6k_large));
}

/*
 * Our virtual destructor.
 */
template<typename T>
grfirfilt_impl<T>::~grfirfilt_impl() {}

template<typename T>
int grfirfilt_impl<T>::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    return 0;
}

template<>
int grfirfilt_impl<float>::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    auto in = static_cast<const float*>(input_items[0]);
    auto out = static_cast<float*>(output_items[0]);

    auto apIn = util::make_aligned_ptr<float>(noutput_items, in);
    util::aligned_ptr<float> apOut { };

    d_FilterF->filter(apIn, apOut);

    std::memcpy(out, apOut.get(), apOut.size() * sizeof(float));

    // Tell runtime system how many output items we produced.
    return apOut.size();
}

template<>
int grfirfilt_impl<gr_complex>::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    auto in = static_cast<const gr_complex*>(input_items[0]);
    auto out = static_cast<gr_complex*>(output_items[0]);

    auto apIn = util::make_aligned_ptr<gr_complex>(noutput_items, in);
    util::aligned_ptr<gr_complex> apOut { };

    d_FilterC->filter(apIn, apOut);

    std::memcpy(out, apOut.get(), apOut.size() * sizeof(gr_complex));

    // Tell runtime system how many output items we produced.
    return apOut.size();
}

template class grfirfilt<float>;
template class grfirfilt<gr_complex>;

} /* namespace radiomon */
} /* namespace gr */
