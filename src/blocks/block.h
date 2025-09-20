// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <functional>
#include <type_traits>

#include "aligned_ptr.h"

namespace dsp {

template<typename>
struct is_block_func : public std::false_type {};

template<>
struct is_block_func<void(const util::aligned_ptr<float>&, util::aligned_ptr<float>&)> :
        public std::true_type {};

template<>
struct is_block_func<void(const util::aligned_ptr<float>&, util::aligned_ptr<std::complex<float>>&)> :
        public std::true_type {};

template<>
struct is_block_func<void(const util::aligned_ptr<std::complex<float>>&, util::aligned_ptr<std::complex<float>>&)> :
        public std::true_type {};

template<>
struct is_block_func<void(const util::aligned_ptr<std::complex<float>>&, util::aligned_ptr<float>&)> :
        public std::true_type {};

template<typename T>
constexpr bool is_block_func_v = is_block_func<T>::value;

using func_ff = void(const util::aligned_ptr<float>&, util::aligned_ptr<float>&);
using func_fc = void(const util::aligned_ptr<float>&, util::aligned_ptr<std::complex<float>>&);
using func_cc = void(const util::aligned_ptr<std::complex<float>>&, util::aligned_ptr<std::complex<float>>&);
using func_cf = void(const util::aligned_ptr<std::complex<float>>&, util::aligned_ptr<float>&);

template<typename T>
class block
{
public:
    block() { }
    block(const block &) = delete;
    block& operator=(const block &) = delete;

    auto get_processer() { return process; }

protected:

    std::function<T> process;
};
}
