// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdio.h>

#include "zmq/sample-msg.h"
#include "timer.h"
#include "cmdline.h"
#include "test-zmq.h"

using namespace util::zmq;

int main(int argc, char **argvp)
{
    // Fix up the 'HDR' type
#ifdef TEST_ZMQ_HDR_STR
    auto HDR = test_zmq_vars::HDR;
#else
    const auto HDR = static_cast<std::array<uint8_t, test_zmq_vars::arr_size>>(test_zmq_vars::HDR);
#endif

    sample_msg<sample_t, SUB_EP, test_zmq_vars::arr_size> sm { HDR };

    sm.init(test_zmq_vars::EPID);

    util::aligned_ptr<sample_t> buff;

    FILE *f = fopen("test-zmq-subscribe.txt", "w");

    while(1)
    {
        int rc = sm.recv(buff);

        if (!rc)
        {
            printf("Rx: %lu\n", buff.size());

        #ifdef TEST_ZMQ_FLOAT
            util::printReal(f, buff.size(), buff.data());
        #else
            util::printComplex(f, buff.size(), buff.data());
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
