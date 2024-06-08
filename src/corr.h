#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include "dmr-mon.h"

namespace dmr {

class corr
{
public:
    enum mode_t
    {
        MODE_BS,    // correlate base station signals
        MODE_MS     // correlate mobile station signals
    };

    corr();         // defauls to BS signals
    corr(mode_t mode);

    virtual ~corr();

    corr(const corr&) = delete;
    corr& operator=(const corr&) = delete;

    const int SYMBOL_Q_SIZE     = 1024;

    void pushSymbols(std::size_t size, float *syms)
    {
        m_SymbolQ.push(syms, size);
    }

private:
    bool m_Running;
    mode_t m_Mode;

    boost::lockfree::spsc_queue<float> m_SymbolQ;

    symbol_t m_SyncTab[SYNC_WORD_SYMBOL_NUM * SAMPLES_PER_SYMBOL];
    symbol_t m_CorrBuff[SYNC_WORD_SYMBOL_NUM * SAMPLES_PER_SYMBOL];

    static const int SYMBUFF_SIZE = TDMA_FRAME_SYMBOL_NUM * 2;

    symbol_t *m_SymBuffP;
    symbol_t *m_SymBuffBurstStart;
    symbol_t *m_SymBuffEnd;
    symbol_t m_SymBuff[SYMBUFF_SIZE];

    void buildSymTable();
    static void handleSymbols(corr *inst);
};

}