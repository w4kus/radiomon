#include <unistd.h>
#include <zmq.hpp>
#include <getopt.h>

#include "dmr-mon.h"
#include "sync.h"
#include "log.h"

#define MSEC(x) (x*1000)

static dmr::symbol_t rxBuff[128];

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
    dmr::corr synchronize(dmr::corr::MODE_BS, (dmr::corr::corr_callback_t)&decode_callback);

    if (!parseCmdLine(argc, args))
        return -1;

    dmr::log::getInst()->SetLogActiveMods(dmr::log::ALL);

    try
    {
        zmq::context_t ctx;
        zmq::socket_t sock(ctx, ZMQ_PULL);
        zmq::mutable_buffer mBuff(rxBuff, sizeof(rxBuff) * sizeof(dmr::symbol_t));
        zmq::recv_buffer_result_t res;

        // Fixme - make this an argument
        sock.connect("tcp://localhost:55000");

        TRACE(dmr::log::MAIN, "Waiting for data...\n");

        while(1)
        {
            res = sock.recv(mBuff);

            if (res.has_value())
            {
                int sz = res.value().size / sizeof(dmr::symbol_t);
                auto *p = (dmr::symbol_t *)mBuff.data();

                if (app_data.dumpSamples)
                {
                    TRACE(dmr::log::MAIN, "\n:: %d\n", sz);

                    for (int i=0;i < sz;i++)
                        TRACE(dmr::log::MAIN, "%.04f\n", *p++);
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
    catch(dmr::util::ringbuffexception &err)
    {
        fprintf(stderr, "ring_buffer Error: %s\n", err.what());
        ret = -1;
    }

    return ret;
}

static void decode_callback(int corrRes, uint8_t *decRes)
{
    TRACE(dmr::log::MAIN, "decode - %d\n", corrRes);
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