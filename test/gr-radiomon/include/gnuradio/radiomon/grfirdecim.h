/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RADIOMON_GRFIRDECIM_H
#define INCLUDED_RADIOMON_GRFIRDECIM_H

#include <gnuradio/radiomon/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace radiomon {

/*!
 * \brief <+description of block+>
 * \ingroup radiomon
 *
 */
template<typename T>
class RADIOMON_API grfirdecim : virtual public gr::sync_decimator
{
public:
    typedef std::shared_ptr<grfirdecim<T>> sptr;

    static sptr make(const short decimate);

    virtual short decimate() const = 0;
};

} // namespace radiomon
} // namespace gr

#endif /* INCLUDED_RADIOMON_GRFIRDECIM_H */
