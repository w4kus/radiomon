/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RADIOMON_GRCARRIER_SYNC_H
#define INCLUDED_RADIOMON_GRCARRIER_SYNC_H

#include <gnuradio/radiomon/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace radiomon {

/*!
 * \brief <+description of block+>
 * \ingroup radiomon
 *
 */
class RADIOMON_API grcarrier_sync : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<grcarrier_sync> sptr;

    static sptr make(const float Kp, const float Ki);

    virtual float Kp() const = 0;
    virtual float Ki() const = 0;
};

} // namespace radiomon
} // namespace gr

#endif /* INCLUDED_RADIOMON_GRCARRIER_SYNC_H */
