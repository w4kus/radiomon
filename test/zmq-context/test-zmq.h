// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#define TEST_ZMQ_FLOAT
#define TEST_ZMQ_HDR_STR

#ifdef TEST_ZMQ_FLOAT
using sample_t = float;
#else
using sample_t = rm_math::complex_f;
#endif

struct test_zmq_vars
{
#ifdef TEST_ZMQ_HDR_STR
    static constexpr uint8_t arr_size = 0;
    static constexpr char const *HDR = "SAMPLES";
#else
    static constexpr uint8_t arr_size = 4;
    static constexpr std::array<uint8_t, arr_size> HDR { 0x0A, 0x0B, 0x0C, 0x0D };
#endif

    static constexpr char const *EPID = "zmq-test";
};

