/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "grfirdecim_impl.h"
#include <gnuradio/io_signature.h>
#include "../radiomon/utils/aligned_ptr.h"

namespace gr {
namespace radiomon {

const float lp_hamming_8p5K[65] =
{
	-0.000645, 0.000135, 0.000898, 0.000754, -0.000445, -0.001570, -0.001046, 0.001195,
    0.002836, 0.001296, -0.002716, -0.004727, -0.001152, 0.005367, 0.007170, 0.000123,
    -0.009549, -0.009988, 0.002475, 0.015807, 0.012921, -0.007732, -0.025188, -0.015658,
    0.017936, 0.040562, 0.017889, -0.040379, -0.073915, -0.019348, 0.125724, 0.284391,
    0.353157, 0.284391, 0.125724, -0.019348, -0.073915, -0.040379, 0.017889, 0.040562,
    0.017936, -0.015658, -0.025188, -0.007732, 0.012921, 0.015807, 0.002475, -0.009988,
    -0.009549, 0.000123, 0.007170, 0.005367, -0.001152, -0.004727, -0.002716, 0.001296,
    0.002836, 0.001195, -0.001046, -0.001570, -0.000445, 0.000754, 0.000898, 0.000135,
    -0.000645
};

constexpr size_t tapNum = sizeof(lp_hamming_8p5K) / sizeof(lp_hamming_8p5K[0]);

template<typename T>
typename grfirdecim<T>::sptr grfirdecim<T>::make(short decimate)
{
    return gnuradio::make_block_sptr<grfirdecim_impl<T>>(decimate);
}


/*
 * The private constructor
 */
template<typename T>
grfirdecim_impl<T>::grfirdecim_impl(const short decimate)
    : gr::sync_decimator("grfirdecim",
                         gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(T)),
                         gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(T)),
                         decimate /*<+decimation+>*/),
                         d_decimate { decimate }
{
}

template<>
grfirdecim_impl<float>::grfirdecim_impl(const short decimate)
    : gr::sync_decimator("grfirdecim",
                         gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(float)),
                         gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(float)),
                         decimate /*<+decimation+>*/),
                         d_decimate { decimate }
{
    d_decimF = std::make_unique<dsp::firdecim_ff>(decimate, util::make_aligned_ptr<float>(tapNum, lp_hamming_8p5K));
}

template<>
grfirdecim_impl<gr_complex>::grfirdecim_impl(const short decimate)
    : gr::sync_decimator("grfirdecim",
                         gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(gr_complex)),
                         gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(gr_complex)),
                         decimate /*<+decimation+>*/),
                         d_decimate { decimate }
{
    d_decimC = std::make_unique<dsp::firdecim_cc>(decimate, util::make_aligned_ptr<float>(tapNum, lp_hamming_8p5K));
}

/*
 * Our virtual destructor.
 */
template<typename T>
grfirdecim_impl<T>::~grfirdecim_impl() {}

template<typename T>
int grfirdecim_impl<T>::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    return 0;
}

template<>
int grfirdecim_impl<float>::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    auto in = static_cast<const float*>(input_items[0]);
    auto out = static_cast<float*>(output_items[0]);

    auto apIn = util::make_aligned_ptr<float>(noutput_items * d_decimate, in);
    util::aligned_ptr<float> apOut { };

    d_decimF->decim(apIn, apOut);

    std::memcpy(out, apOut.get(), apOut.size() * sizeof(float));

    // Tell runtime system how many output items we produced.
    return apOut.size();
}

template<>
int grfirdecim_impl<gr_complex>::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    auto in = static_cast<const gr_complex*>(input_items[0]);
    auto out = static_cast<gr_complex*>(output_items[0]);

    auto apIn = util::make_aligned_ptr<gr_complex>(noutput_items * d_decimate, in);
    util::aligned_ptr<gr_complex> apOut { };

    d_decimC->decim(apIn, apOut);

    std::memcpy(out, apOut.get(), apOut.size() * sizeof(gr_complex));

    // Tell runtime system how many output items we produced.
    return apOut.size();
}

template class grfirdecim<float>;
template class grfirdecim<gr_complex>;

} /* namespace radiomon */
} /* namespace gr */
