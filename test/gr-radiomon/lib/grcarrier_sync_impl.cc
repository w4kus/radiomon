/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "grcarrier_sync_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace radiomon {

grcarrier_sync::sptr grcarrier_sync::make(const float Kp, const float Ki)
{
    return gnuradio::make_block_sptr<grcarrier_sync_impl>(Kp, Ki);
}


/*
 * The private constructor
 */
grcarrier_sync_impl::grcarrier_sync_impl(const float Kp, const float Ki)
    : gr::sync_block("grcarrier_sync",
                     gr::io_signature::make(
                         1 /* min inputs */, 1 /* max inputs */, sizeof(gr_complex)),
                     gr::io_signature::make2(
                         1 /* min outputs */, 2 /*max outputs */, sizeof(gr_complex), sizeof(float))),
                    d_Kp { Kp },
                    d_Ki { Ki }
{
    d_carrier_sync = std::make_unique<dsp::carrier_sync>(Kp, Ki, 2);
    d_carrier_sync->setSamplingRate(48000);
}

/*
 * Our virtual destructor.
 */
grcarrier_sync_impl::~grcarrier_sync_impl() {}

int grcarrier_sync_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    auto in = static_cast<const gr_complex*>(input_items[0]);
    auto out = static_cast<gr_complex*>(output_items[0]);
    auto eout = static_cast<float*>(output_items[1]);

    auto apIn = util::make_aligned_ptr<gr_complex>(noutput_items, in);
    auto apOut = util::aligned_ptr<gr_complex> { };
    auto apErrOut = util::aligned_ptr<float> { };

    d_carrier_sync->sync(apIn, apOut);
    d_carrier_sync->getErrorSig(apErrOut);

    std::memcpy(out, apOut.get(), apOut.size() * sizeof(gr_complex));
    std::memcpy(eout, apErrOut.get(), apErrOut.size() * sizeof(float));

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace radiomon */
} /* namespace gr */
