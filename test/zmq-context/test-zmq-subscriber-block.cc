// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdio.h>

#include "zmq/sample-msg.h"
#include "test-zmq.h"
#include "aligned-ptr.h"
#include "zmq-sample-pub.h"
#include "cmdline.h"
#include "timer.h"

using namespace util::zmq;

int main(int argc, char **argvp)
{
    sample_msg<sample_t, SUB_EP, 0> sm;
    std::array<uint8_t, 5> hdr;
    util::aligned_ptr<sample_t> samples;

    FILE *f = fopen("test-zmq-subscribe-block.txt", "w");

    sm.init(test_zmq_vars::EPID);

    while(1)
    {
        int rc = sm.recv<5>(hdr, samples);

        if (!rc)
        {
            dsp::value_rate_t rate;

            rate.bytes[0] = hdr[1];
            rate.bytes[1] = hdr[2];
            rate.bytes[2] = hdr[3];
            rate.bytes[3] = hdr[4];

            printf("type %02X, rate %u\n", hdr[0], ntohl(rate.val));

        #ifdef TEST_ZMQ_FLOAT
            util::printReal(f, samples.size(), samples.data());
        #else
            util::printComplex(f, samples.size(), samples.data());
        #endif
        }
        else if (rc == -1)
        {
            if (errno != EAGAIN)
            {
                printf("Error on recv..abort [%d]\n", errno);
                break;
            }
        }

        util::timer::sleep(10);
    }

    fclose(f);

    return 0;
}
