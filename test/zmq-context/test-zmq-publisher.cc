// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <thread>

#include "zmq-sample-pub.h"
#include "timer.h"
#include "menu.h"
#include "aligned-ptr.h"
#include "sine-source.h"
#include "timer.h"
#include "chain.h"
#include "signal-source.h"
#include "callback-sink.h"
#include "test-zmq.h"

using namespace util::zmq;

constexpr uint32_t Fs = 48000;
constexpr uint32_t F = 800;
constexpr uint32_t sampleSize = Fs / F;

#ifdef TEST_ZMQ_FLOAT
    using sig_func      = dsp::endpoints::signal_source_ff;
    using cb_func       = dsp::endpoints::callback_ff;

    #ifdef TEST_ZMQ_HDR_STR
        using zmq_func_snk  = dsp::zmq_sample_pub_ff_snk;
        using zmq_func_op   = dsp::zmq_sample_pub_ff_op;
    #else
        using zmq_func_snk  = dsp::zmq_sample_pub_ff_snk_array<4>;
        using zmq_func_op   = dsp::zmq_sample_pub_ff_op_array<4>;
    #endif
#else
    using sig_func      = dsp::endpoints::signal_source_cc;
    using cb_func       = dsp::endpoints::callback_cc;

    #ifdef TEST_ZMQ_HDR_STR
        using zmq_func_snk  = dsp::zmq_sample_pub_cc_snk;
        using zmq_func_op   = dsp::zmq_sample_pub_cc_op;
    #else
        using zmq_func_snk  = dsp::zmq_sample_pub_cc_snk_array<4>;
        using zmq_func_op   = dsp::zmq_sample_pub_cc_op_array<4>;
    #endif
#endif

static bool running = true;
bool test(const std::string &input);

void callback(const util::aligned_ptr<sample_t> &in)
{
    printf("cb: %lu\n", in.size());
}

int main(int argc, char **argvp)
{
    util::menu::entry entry;
    util::menu menu { "ZMQ TEST" };

    menu.set_help(true);

    entry.key = 'Q';
    entry.prompt = nullptr;
    entry.description = "Quit";
    entry.handler = test;
    menu.add(entry);

    menu.display();

    // Fix up the 'HDR' type
#ifdef TEST_ZMQ_HDR_STR
    auto HDR = test_zmq_vars::HDR;
#else
    const auto HDR = static_cast<std::array<uint8_t, test_zmq_vars::arr_size>>(test_zmq_vars::HDR);
#endif

///// Choose one of the following three tests
// Stand alone test
#if 1
    sample_msg<sample_t, PUB_EP, test_zmq_vars::arr_size> sm { HDR };

    sm.init(test_zmq_vars::EPID);

    util::aligned_ptr<sample_t> buff;
    util::init_aligned_ptr_on_resize(buff, sampleSize);

    util::sine_source<sample_t> sig { rm_math::hz_to_rps(F, Fs) };

    while(running)
    {
        sig.get(buff);
        sm.send(buff);

        menu.processInput();

        util::timer::sleep(10);
    }
#endif

// In-chain as an operator test
#if 0
    util::chain chain { "ZMQ" };

    chain.add(std::make_unique<sig_func>(sampleSize, F, Fs), "SIG_SOURCE");
    chain.add(std::make_unique<zmq_func_snk>(static_cast<const char *>(test_zmq_vars::EPID), HDR), "ZMQ_SNK");

    assert(chain.setup());

    while(running)
    {
        chain.iterate();
        menu.processInput();
        util::timer::sleep(10);
    }
#endif

// In-chain as a sink test
#if 0
    util::chain chain { "ZMQ" };

    chain.add(std::make_unique<sig_func>(sampleSize, F, Fs), "SIG_SOURCE");
    chain.add(std::make_unique<zmq_func_snk>(static_cast<const char *>(test_zmq_vars::EPID), HDR), "ZMQ_OP");
    chain.add(std::make_unique<cb_func>(callback), "CB_SNK");

    assert(chain.setup());

    while(running)
    {
        chain.iterate();
        menu.processInput();
        util::timer::sleep(10);
    }
#endif

    return 0;
}

bool test(const std::string &input)
{
    running = false;
    return false;
}
