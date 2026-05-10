// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdlib.h>
#include <thread>
#include <stdio.h>

#include "timer.h"
#include "cmdline.h"
#include "ring-buffer.h"

static FILE *f, *g;
static std::atomic_bool kill;

void theThread(util::ring_buffer<uint32_t> &rb)
{
    uint32_t buff[4];

    while(1)
    {
        rb.read(buff, 5);

        if (!rb.abortcode())
        {
            fprintf(g, "read: ");
            for (size_t i=0;i < 5;i++)
                fprintf(g, "%u ", buff[i]);
            fprintf(g, "\n");
        }
        else
        {
            printf("abort: %d\n", rb.abortcode());
            break;
        }
    }
}

int main(int argc, char **argvp)
{
    util::ring_buffer<uint32_t> test(8);
    std::thread th(theThread, std::ref(test));

    f = fopen("write.txt", "w");
    g = fopen("read.txt", "w");

    int i = 100000;
    while(i--)
    {
        uint32_t buff[10] = { 1,2,3,4,5,6,7,8,9,10 };

        fprintf(f, "write: ");
        for (size_t i=0;i < 10;i++)
            fprintf(f, "%u ", buff[i]);
        fprintf(f, "\n");

        test.write(buff, 10);
    }

    while(test.amount())
        util::timer::sleep(500);

    kill = true;
    test.abort();
    th.join();
    util::ring_buffer<uint32_t>::diag diag;
    test.diagnostics(diag);

    printf("fc=%u ec=%u\n", diag.fullCount, diag.emptyCount);
    fclose(f);
    fclose(g);

    return 0;
}
