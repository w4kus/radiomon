#pragma once

#include "ring-buffer.h"
#include "dmr-mon.h"
#include "peak.h"
#include <functional>

namespace dmr {

class sync
{
public:
    typedef enum
    {
        MODE_BS,    // correlate base station signals
        MODE_MS,    // correlate mobile station signals
        MODE_TS1,   // correlate direct TS1 signals
        MODE_TS2,   // correlate direct TS2 signals
        MODE_MAX
    }frame_t;

    typedef std::function<void(int size, uint8_t *buff)> corr_callback_t;

    sync();
    sync(corr_callback_t cb);
    sync(frame_t mode, bool test = false);
    sync(frame_t mode, corr_callback_t cb, bool test = false);

    ~sync();

    sync(const sync&) = delete;
    sync& operator=(const sync&) = delete;

    void pushSymbols(std::size_t size, symbol_t *syms)
    {
        m_SymbolRingBuff.pushBuffer(syms, size);
    }

    void changeMode(mode_t newMode);

protected:

    enum sync_state_t
    {
        ST_SEARCH,
        ST_SYNC,
    };

    int m_Running;
    mode_t m_Mode;
    sync_state_t m_State;

    util::peak<symbol_t> m_Peak;

    util::ring_buffer<symbol_t> m_SymbolRingBuff;

    static constexpr int SYNC_WORD_SIZE     = SYNC_WORD_SYMBOL_NUM * SAMPLES_PER_SYMBOL;
    static constexpr int XCORR_SIZE         = SYNC_WORD_SIZE * 2 - 1;
    static constexpr int FRAME_SIZE         = BURST_FRAME_SYMBOL_NUM * SAMPLES_PER_SYMBOL;
    static constexpr int CORR_BUFF_SIZE     = FRAME_SIZE * 2;
    static constexpr int PAYLOAD_SIZE       = BURST_PAYLOAD_SECT_SYMBOL_NUM * SAMPLES_PER_SYMBOL;

    static constexpr symbol_t SYNC_ERROR    = PERCENT_TO_VALUE(3.0f);
    static constexpr symbol_t XCORR_MAX     = (symbol_t)SYNC_WORD_SIZE + (symbol_t)(SYNC_WORD_SIZE * SYNC_ERROR);
    static constexpr symbol_t XCORR_MIN     = (symbol_t)SYNC_WORD_SIZE - (symbol_t)(SYNC_WORD_SIZE * SYNC_ERROR);
    static constexpr symbol_t SYNC_MAX      = 0.0f + (symbol_t)(1.0f * SYNC_ERROR);
    static constexpr symbol_t SYNC_MIN      = 0.0f - (symbol_t)(1.0f * SYNC_ERROR);

    symbol_t m_SyncTab[SYNC_WORD_SIZE];
    symbol_t m_CorrBuff[CORR_BUFF_SIZE];
    symbol_t m_SyncSum;
    symbol_t m_CurrSym;
    size_t   m_CorrBuffCount;

    uint16_t m_CorrBuffIdxW;
    uint16_t m_CorrBuffIdxR;

    corr_callback_t m_DecodeCB;

    void buildSymTable();
    void commonStartup();
    void stopThread();

    bool correlate();
    void startSync();
    bool inSync();
    void decode();

    static bool isInPosRange(const symbol_t sym, const symbol_t max, const symbol_t min)
    {
        return (sym >= min) && (sym <= max);
    }

    static bool isInNegRange(symbol_t sym, const symbol_t max, const symbol_t min)
    {
        return (sym <= -min) && (sym >= -max);
    }

    static bool isSumInRange(symbol_t sum)
    {
        return (sum >= SYNC_MIN) && (sum <= SYNC_MAX);
    }

    void xcorr(symbol_t *buff, symbol_t &max, symbol_t &min);

private:

    static void handleSymbols(sync *inst);
};

}

// For unit testing and such
extern const dmr::symbol_t sym_table_bs_source_voice[];
extern const dmr::symbol_t sym_table_ms_source_voice[];
extern const dmr::symbol_t sym_table_direct_ts1_voice[];
extern const dmr::symbol_t sym_table_direct_ts2_voice[];
