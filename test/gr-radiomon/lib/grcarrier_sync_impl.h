/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RADIOMON_GRCARRIER_SYNC_IMPL_H
#define INCLUDED_RADIOMON_GRCARRIER_SYNC_IMPL_H

#include <gnuradio/radiomon/grcarrier_sync.h>
#include "../radiomon/blocks/carrier-sync.h"
#include <memory.h>

namespace gr {
namespace radiomon {

class grcarrier_sync_impl : public grcarrier_sync
{
private:
    
    float d_Kp;
    float d_Ki;

    std::unique_ptr<dsp::carrier_sync> d_carrier_sync;

public:
    grcarrier_sync_impl(const float Kp, const float Ki);
    ~grcarrier_sync_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    float Kp() const { return d_Kp; }
    float Ki() const { return d_Ki; }
};

} // namespace radiomon
} // namespace gr

#endif /* INCLUDED_RADIOMON_GRCARRIER_SYNC_IMPL_H */
