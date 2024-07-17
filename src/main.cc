#include <unistd.h>
#include <zmq.hpp>

#include "dmr-mon.h"
#include "corr.h"
#include "log.h"

#define MSEC(x) (x*1000)

const int RX_BUFF_SIZE = dmr::TDMA_FRAME_SYMBOL_NUM * dmr::SAMPLES_PER_SYMBOL;
static dmr::symbol_t rxBuff[RX_BUFF_SIZE];

int main(int argc, char **args)
{
    int ret(0);

    dmr::log::getInst()->SetLogActiveMods(dmr::log::ALL);

#if 1
    dmr::corr corr;
    corr.test();
#else
    try
    {
        zmq::context_t ctx;
        zmq::socket_t sock(ctx, ZMQ_PULL);
        zmq::mutable_buffer mBuff(rxBuff, RX_BUFF_SIZE * sizeof(dmr::symbol_t));
        zmq::recv_buffer_result_t res;
        dmr::corr corr;

        sock.connect("tcp://localhost:55000");

        dmr::log::getInst()->trace(dmr::log::MAIN, "Waiting for data...\n");

        while(1)
        {
            res = sock.recv(mBuff);

            if (res.has_value())
            {
                int sz = res.value().size / sizeof(dmr::symbol_t);

                dmr::log::getInst()->trace(dmr::log::MAIN, "\n:: %d\n", sz);

                auto *p = (dmr::symbol_t *)mBuff.data();

                for (int i=0;i < sz;i++)
                    dmr::log::getInst()->trace(dmr::log::MAIN, "%.04f\n", *p++);
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
    catch(dmr::error &err)
    {
        fprintf(stderr, "DMR Error: %s\n", err.what());
        ret = err.code;
    }
#endif

    return ret;
}
