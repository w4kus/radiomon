/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RADIOMON_GRFIRINTERP_IMPL_H
#define INCLUDED_RADIOMON_GRFIRINTERP_IMPL_H

#include <gnuradio/radiomon/grfirinterp.h>
#include "../radiomon/blocks/firinterp.h"

namespace gr {
namespace radiomon {

template<typename T>
class grfirinterp_impl : public grfirinterp<T>
{
private:

    std::unique_ptr<dsp::firinterp_ff> d_InterpF;
    std::unique_ptr<dsp::firinterp_cc> d_InterpC;

    short d_interpolate;
    bool d_adjustGain;

public:

    grfirinterp_impl(short interpolate, bool adjustGain);
    ~grfirinterp_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    short interpolate() const { return d_interpolate; }
    bool adjustGain() const { return d_adjustGain; }
};

} // namespace radiomon
} // namespace gr

#endif /* INCLUDED_RADIOMON_GRFIRINTERP_IMPL_H */
