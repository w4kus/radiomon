/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RADIOMON_GRFIRINTERP_H
#define INCLUDED_RADIOMON_GRFIRINTERP_H

#include <gnuradio/radiomon/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace radiomon {

/*!
 * \brief <+description of block+>
 * \ingroup radiomon
 *
 */
template<typename T>
class RADIOMON_API grfirinterp : virtual public gr::sync_interpolator
{
public:
    typedef std::shared_ptr<grfirinterp<T>> sptr;

    static sptr make(short interpolate, bool adjustGain);

    virtual short interpolate() const = 0;
    virtual bool adjustGain() const = 0;
};

} // namespace radiomon
} // namespace gr

#endif /* INCLUDED_RADIOMON_GRFIRINTERP_H */
