// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

// #include "zmq/sample-stream.h"
// #include "aligned-ptr.h"
#include "timer.h"
#include "test-zmq.h"
#include "zmq-stream-source.h"
#include "chain.h"
#include "callback-sink.h"

using namespace util::zmq;

void sampleCb(const util::aligned_ptr<float> &data)
{
    printf("Recv %ld\n", data.size());

    for (size_t i=0;i < data.size();i++)
        printf("%02f\n", data[i]);
    printf("\n\n");
}

int main(int argc, char **argvp)
{
#if 0
    sample_stream<sample_t, SUB_EP> ss;
    ss.setURL();

    auto data = util::make_aligned_ptr<sample_t>(32);

#ifdef TEST_ZMQ_FLOAT
    std::fill(data.begin(), data.end(), 0);
#else
    std::fill(data.begin(), data.end(), rm_math::complex_f(0.0f, 0.0f));
#endif

    while(1)
    {
        auto rec = ss.recv(data);

        if (rec == -1)
        {
            printf("Error receiving - %d\n", errno);
            break;
        }

        printf("Recv %d\n", rec);

        if (rec > 0)
        {
        #ifdef TEST_ZMQ_FLOAT
            for (size_t i=0;i < data.size();i++)
                printf("%.06f ", data[i]);

            std::fill(data.begin(), data.end(), 0);
        #else
            for (size_t i=0;i < data.size();i++)
                printf("[%.06f %06f] ", data[i].real(), data[i].imag());

            std::fill(data.begin(), data.end(), rm_math::complex_f(0.0f, 0.0f));
        #endif
            printf("\n");

        }

        util::timer::sleep(10);
    }
#else

    util::chain theChain("ZMQ_STRM_SRC");

    theChain.add(std::make_unique<dsp::endpoints::zmq_stream_source_ff>("tcp://127.0.0.1:55555"), "STRM_SRC");
    theChain.add(std::make_unique<dsp::endpoints::callback_ff>(sampleCb), "CB_SNK");

    if (!theChain.setup())
    {
        printf("Chain setup failed\n");
        return -1;
    }

    while(1)
    {
        theChain.iterate();
        util::timer::sleep(10);
    }


#endif

    return 0;
}
