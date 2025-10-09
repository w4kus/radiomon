/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RADIOMON_GRFIRDECIM_IMPL_H
#define INCLUDED_RADIOMON_GRFIRDECIM_IMPL_H

#include <gnuradio/radiomon/grfirdecim.h>
#include <memory>
#include "../radiomon/blocks/firdecim.h"

namespace gr {
namespace radiomon {

template<typename T>
class grfirdecim_impl : public grfirdecim<T>
{
private:

    std::unique_ptr<dsp::firdecim_ff> d_decimF;
    std::unique_ptr<dsp::firdecim_cc> d_decimC;

    short d_decimate;

public:
    grfirdecim_impl(const short decimate);
    ~grfirdecim_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    short decimate() const { return d_decimate; }
};

} // namespace radiomon
} // namespace gr

#endif /* INCLUDED_RADIOMON_GRFIRDECIM_IMPL_H */
