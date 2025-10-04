/* -*- c++ -*- */
/*
 * Copyright 2025 John Mark White.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RADIOMON_GRFIRFILT_H
#define INCLUDED_RADIOMON_GRFIRFILT_H

#include <gnuradio/radiomon/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace radiomon {

/*!
 * \brief Test the radio-mon firfilt module
 * \ingroup radiomon
 *
 */
template<typename T>
class RADIOMON_API grfirfilt : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<grfirfilt<T>> sptr;

    static sptr make();
};

} // namespace radiomon
} // namespace gr

#endif /* INCLUDED_RADIOMON_GRFIRFILT_H */
