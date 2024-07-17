#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <fftw3.h>
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

    enum convolve_t
    {
        CONVOLVE_DIRECT,
        CONVOLVE_FFT
    };

    corr();         // defauls to BS signals, direct convolution
    corr(mode_t mode, convolve_t convType);

    virtual ~corr();

    corr(const corr&) = delete;
    corr& operator=(const corr&) = delete;

    const int SYMBOL_Q_SIZE     = 1024;

    void pushSymbols(std::size_t size, float *syms)
    {
        m_SymbolRingBuff.push(syms, size);
    }

    // TEMP DEBUG
    void test();

private:

    bool m_Running;
    mode_t m_Mode;

    boost::lockfree::spsc_queue<symbol_t> m_SymbolRingBuff;

    static const int SYMBUFF_SIZE = TDMA_FRAME_SYMBOL_NUM * SAMPLES_PER_SYMBOL * 2;
    static const int SYNC_WORD_SIZE = SYNC_WORD_SYMBOL_NUM * SAMPLES_PER_SYMBOL;
    static const int CONVOLVE_SIZE = SYNC_WORD_SIZE * 2 - 1;

    symbol_t m_SyncTab[SYNC_WORD_SIZE];
    symbol_t m_CorrBuff[CONVOLVE_SIZE];

    symbol_t *m_SymBuffP;
    symbol_t *m_SymBuffBurstStart;
    symbol_t *m_SymBuffEnd;
    symbol_t m_SymBuff[SYMBUFF_SIZE];
    size_t m_SymBuffCount;
    fftw_plan m_FFTFwdPlan;
    fftw_plan m_FFTBackPlan;

    bool m_IsCorrelated;

    void buildSymTable();
    void commonStartup();

    size_t next(size_t n = 1);
    bool correlate();
    bool accum();
    void decode();

    symbol_t directConvolve();
    symbol_t fftConvolve();

    static void handleSymbols(corr *inst);

    symbol_t testBuff[CONVOLVE_SIZE];
};

}