// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include "aligned_ptr.h"

namespace dsp {

class block
{
public:
    block() { }
    block(const block &) = delete;
    block& operator=(const block &) = delete;
};
}
