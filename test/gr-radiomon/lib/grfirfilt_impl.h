/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RADIOMON_GRFIRFILT_IMPL_H
#define INCLUDED_RADIOMON_GRFIRFILT_IMPL_H

#include <gnuradio/radiomon/grfirfilt.h>
#include "../radiomon/blocks/firfilt.h"

namespace gr {
namespace radiomon {

template<typename T>
class grfirfilt_impl : public grfirfilt<T>
{
private:

    // Since the radio-mon's blocks require a second template paramter
    // of a type compatible with std::function, and I was unable to
    // get it to work with pybind, we'll declare two versions of
    // dsp::firfilter, one float and one complex, and use template
    // specializations.
    std::unique_ptr<dsp::firfilter_ff> d_FilterF;
    std::unique_ptr<dsp::firfilter_cc> d_FilterC;

public:
    grfirfilt_impl();
    ~grfirfilt_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace radiomon
} // namespace gr

#endif /* INCLUDED_RADIOMON_GRFIRFILT_IMPL_H */
