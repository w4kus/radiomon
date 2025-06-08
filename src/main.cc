#include <unistd.h>
#include <getopt.h>

#include "dmr-mon.h"
#include "sync.h"
#include "log.h"
#include "zmq_stream.h"

#define MSEC(x) (x*1000)

#define DEBUG(fmt,...) TRACE(dmr::log::MAIN, fmt, ##__VA_ARGS__)

// static dmr::symbol_t rxBuff[128];

typedef struct
{
    bool dumpSamples;
}app_data_t;

static app_data_t app_data;

static void decode_callback(int corrRes, uint8_t *decRes);
static bool parseCmdLine(int argc, char **argv);

int main(int argc, char **args)
{
    int ret(0);
    dmr::sync synchronize(dmr::sync::MODE_BS, (dmr::sync::corr_callback_t)&decode_callback);

    if (!parseCmdLine(argc, args))
        return -1;

    // dmr::log::getInst()->SetLogActiveMods(dmr::log::ALL);

    try
    {
        util::zmq_stream<dmr::symbol_t> stream(ZMQ_PULL, 8192);

        // FIXME - make this an argument
        stream.sock().connect("tcp://localhost:55000");

        DEBUG("Waiting for data...\n");

        while(1)
        {
            auto res = stream.recv();

            if (res.has_value())
            {
                auto sz = stream.size(res);
                auto *p = (dmr::symbol_t *)stream.data();

                if (app_data.dumpSamples)
                {
                    // printf("\n:: %d\n", sz);

                    for (int i=0;i < sz;i++)
                    {
                        printf("%0.04f\n", *p);
                        ++p;
                    }
                }
                else
                    synchronize.pushSymbols(sz, p);
            }
            else
            {
                ret = -1;
                break;
            }
        }
    }
    catch(zmq::error_t &err)
    {
        fprintf(stderr, "ZMQ Error: %s\n", err.what());
        ret = -1;
    }
    catch(util::ringbuffexception &err)
    {
        fprintf(stderr, "ring_buffer Error: %s\n", err.what());
        ret = -1;
    }

    return ret;
}

static void decode_callback(int corrRes, uint8_t *decRes)
{
    DEBUG("decode - %d\n", corrRes);
}

static bool parseCmdLine(int argc, char **argv)
{
    int opt;
    bool ret = true;

    while((opt = getopt_long(argc, argv, "d", nullptr, nullptr)) != -1)
    {
        switch (opt)
        {
            case 'd':
                app_data.dumpSamples = true;
                break;

            case '?':
                printf("FIXME - print help\n");
                ret = false;
        }
    }

    return ret;
}
