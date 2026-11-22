// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "zmq/sample-stream.h"
#include "aligned-ptr.h"
#include "timer.h"
#include "test-zmq.h"

using namespace util::zmq;

int main(int argc, char **argvp)
{
    sample_stream<sample_t, PUB_EP> ss;
    ss.setURL();

    auto data = util::make_aligned_ptr<sample_t>(32);

    for (size_t i=0;i < data.size();i++)
        data[i] = static_cast<float>(i);

    while(1)
    {
        auto sent = ss.send(data);

        if (sent == -1)
        {
            printf("Error sending - %d\n", errno);
            break;
        }

        printf("Sent %d\n", sent);

        util::timer::sleep(10);
    }

    return 0;
}
