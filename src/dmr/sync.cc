#include "sync.h"
#include "log.h"
#include <cstring>
#include <boost/assert.hpp>
#include <thread>
#include <unistd.h>

#define DEBUG(fmt,...) TRACE(dmr::log::CORR, fmt, ##__VA_ARGS__)

using namespace dmr;


// BS source sync: voice (compliment: data)
//   7      5      5      F     D      7      D      F      7      5      F       7
// +3 -3 +3 +3  +3 +3  -3 -3  -3 +3  +3 -3  -3 +3  -3 -3  +3 -3  +3 +3  -3 -3  +3 -3
//
// Normalized BS source voice sync symbol table
const symbol_t sym_table_bs_source_voice[] =
{
    +1, -1, +1, +1,     // 75
    +1, +1, -1, -1,     // 5F
    -1, +1, +1, -1,     // D7
    -1, +1, -1, -1,     // DF
    +1, -1, +1, +1,     // 75
    -1, -1, +1, -1      // F7
};

// MS source sync: voice (compliment: data)
//   7      F      7      D      5      D     D      5      7      D      F      D
// +3 -3  -3 -3  +3 -3  -3 +3  +3 +3  -3 +3 -3 +3  +3 +3  +3 -3  -3 +3  -3 -3  -3 +3
//
// Normalized MS source voice sync symbol table
const symbol_t sym_table_ms_source_voice[] =
{
    +1, -1, -1, -1,     // 7F
    +1, -1, -1, +1,     // 7D
    +1, +1, -1, +1,     // 5D
    -1, +1, +1, +1,     // D5
    +1, -1, -1, +1,     // 7D
    -1, -1, -1, +1      // FD
};

// Direct mode voice - TS1 (compliment: data)
//    5     D       5     7       7     F       7      7    5      7      F     F
// +3 +3  -3 +3  +3 +3  +3 -3  +3 -3  -3 -3  +3 -3  +3 -3 +3 +3  +3 -3 -3 -3  -3 -3
//
// Normalized TS1 direct mode
const symbol_t sym_table_direct_ts1_voice[] =
{
    +1, +1, -1, +1,     // 5D
    +1, +1, +1, -1,     // 57
    +1, -1, -1, -1,     // 7F
    +1, -1, +1, -1,     // 77
    +1, +1, +1, -1,     // 57
    -1, -1, -1, -1      // FF
};

// Direct mode voice - TS2 (compliment: data)
//    7      D      F     F       D      5      F      5      5      D      5      F
// +3 -3  -3 +3  -3 -3  -3 -3  -3 +3  +3 +3  -3 -3  +3 +3  +3 +3  -3 +3  +3 +3  -3 -3
//
// Normalized TS2 direct mode
const symbol_t sym_table_direct_ts2_voice[] =
{
    +1, -1, -1, +1,     // 7D
    -1, -1, -1, -1,     // FF
    -1, +1, +1, +1,     // D5
    -1, -1, +1, +1,     // F5
    +1, +1, -1, +1,     // 5D
    +1, +1, -1, -1      // 5F
};

const symbol_t *symbolTabLookup[corr::MODE_MAX] =
{
    [corr::MODE_BS]   = sym_table_bs_source_voice,
    [corr::MODE_MS]   = sym_table_ms_source_voice,
    [corr::MODE_TS1]  = sym_table_direct_ts1_voice,
    [corr::MODE_TS2]  = sym_table_direct_ts2_voice
};

corr::corr() : m_Running(2),
               m_Mode(MODE_BS),
               m_State(ST_SEARCH),
               m_SymbolRingBuff(10),
               m_SyncSum(0),
               m_CorrBuffCount(0),
               m_CorrBuffIdxW(0),
               m_CorrBuffIdxR(0),
               m_DecodeCB(nullptr)
{
    commonStartup();
}

corr::corr(corr_callback_t cb) :    m_Running(2),
                                    m_Mode(MODE_BS),
                                    m_State(ST_SEARCH),
                                    m_SymbolRingBuff(10),
                                    m_SyncSum(0),
                                    m_CorrBuffCount(0),
                                    m_CorrBuffIdxW(0),
                                    m_CorrBuffIdxR(0),
                                    m_DecodeCB(cb)
{
    commonStartup();
}

corr::corr(frame_t mode, bool test) :    m_Running(2),
                                        m_Mode(mode),
                                        m_State(ST_SEARCH),
                                        m_SymbolRingBuff(10),
                                        m_SyncSum(0),
                                        m_CorrBuffCount(0),
                                        m_CorrBuffIdxW(0),
                                        m_CorrBuffIdxR(0),
                                        m_DecodeCB(nullptr)
{
    commonStartup();
}

corr::corr(frame_t mode, corr_callback_t cb, bool test) :    m_Running(2),
                                                            m_Mode(mode),
                                                            m_State(ST_SEARCH),
                                                            m_SymbolRingBuff(10),
                                                            m_SyncSum(0),
                                                            m_CorrBuffCount(0),
                                                            m_CorrBuffIdxW(0),
                                                            m_CorrBuffIdxR(0),
                                                            m_DecodeCB(cb)
{
    commonStartup();
}

corr::~corr()
{
    stopThread();
}

void corr::commonStartup()
{
    // These can throw an excepm_SyncSumtion
    buildSymTable();
    std::thread(corr::handleSymbols, this).detach();
}

void corr::buildSymTable()
{
    int dstIdx = 0, srcIdx = 0;
    auto *srcTab = symbolTabLookup[m_Mode];

    for (int i=0;i < SYNC_WORD_SYMBOL_NUM;i++)
    {
        for (int j=0;j < SAMPLES_PER_SYMBOL;j++)
            m_SyncTab[dstIdx++] = srcTab[srcIdx];
        
        ++srcIdx;
    }

    std::memset(m_CorrBuff, 0, sizeof(m_CorrBuff));
}

void corr::stopThread()
{
    m_Running = 1;
    m_SymbolRingBuff.setBreak();

    while(m_Running)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void corr::changeMode(mode_t newMode)
{
}

////////////////////////////////////////////////////////////////////////////////////
// Local thread context
void corr::handleSymbols(corr *inst)
{
    DEBUG("searching...\n");

    while(inst->m_Running == 2)
    {
        for (int i=0;i < 8;i++)
        {
            if (inst->m_State == ST_SEARCH)
            {
                if (inst->correlate())
                {
                    inst->m_State = ST_SYNC;
                    inst->startSync();
                    TRACE(log::CORR, "ST_SYNC\n");
                }
            }
            else
            {
                if (inst->inSync())
                {
                    inst->decode();
                    inst->m_State = ST_SEARCH;
                    TRACE(log::CORR, "ST_SEARCH\n");
                }
            }
        }
    }

    inst->m_Running = 0;
}

bool corr::correlate()
{
    bool ret = false;
    symbol_t min = 0, max = 0;

    auto sym = m_SymbolRingBuff.pop();

    m_CorrBuff[m_CorrBuffIdxW++] = sym;
    ++m_CorrBuffCount;

    if (m_CorrBuffIdxW == CORR_BUFF_SIZE)
        m_CorrBuffIdxW = 0;

    if (m_CorrBuffCount > CORR_BUFF_SIZE)
    {
        ++m_CorrBuffIdxR;
        if (m_CorrBuffIdxR == CORR_BUFF_SIZE)
            m_CorrBuffIdxR = 0;

        --m_CorrBuffCount;
    }

    if (m_CorrBuffCount >= SYNC_WORD_SIZE)
    {
        int diff = m_CorrBuffIdxW - SYNC_WORD_SIZE;
        int idx = 0;
        symbol_t *p, *q, sum = 0;

        if (diff >= 0)
            idx = diff;
        else
            idx = CORR_BUFF_SIZE - m_CorrBuffIdxW - 1;

        p = q = &m_CorrBuff[idx];

        auto *buff = new symbol_t[XCORR_SIZE]();
        auto *buffEnd = &m_CorrBuff[CORR_BUFF_SIZE];

        for (int i=0;i < SYNC_WORD_SIZE;i++)
        {
            buff[i] = *p;
            sum += *p++;

            if (p == buffEnd)
                p = m_CorrBuff;
        }

        if (isSumInRange(sum))
        {
            // DEBUG("Checking sync [%.2f]\n", sum);

            xcorr(buff, min, max);

            if (isInPosRange(SYNC_WORD_SIZE, min, max))
            {
                DEBUG("---pos sync found [%.2f %.2f]\n", min, max);
                ret = true;
            }
            else if (isInNegRange(SYNC_WORD_SIZE, min, max))
            {
                DEBUG("---neg sync found [%.2f %.2f]\n", min, max);
                ret = true;
            }
        }

        delete[] buff;
    }

    return ret;
}

void corr::startSync()
{
#if 0
    int diff = m_CorrBuffIdxR - PAYLOAD_SIZE;

    if (diff >= 0)
        m_CorrBuffIdxR = diff;
    else
        m_CorrBuffIdxR = CORR_BUFF_SIZE + diff - 1;

    m_CorrBuffCount += PAYLOAD_SIZE + SYNC_WORD_SIZE;
#endif
}

bool corr::inSync()
{
#if 0
    bool ret = false;

    m_CorrBuff[m_CorrBuffIdxW++] = m_SymbolRingBuff.pop();
    ++m_CorrBuffCount;

    if (m_CorrBuffIdxW == CORR_BUFF_SIZE)
        m_CorrBuffIdxW = 0;

    if (m_CorrBuffCount == FRAME_SIZE)
        ret = true;

    return ret;
#else
    m_CorrBuffIdxR = m_CorrBuffIdxW;
    m_CorrBuffCount = 0;
    return true;
#endif
}

void corr::decode()
{
    m_CorrBuffIdxR = m_CorrBuffIdxW;

    if (m_DecodeCB)
        m_DecodeCB(0, nullptr);
}

void corr::xcorr(symbol_t *buff, symbol_t &max, symbol_t &min)
{
    auto *res = new symbol_t[XCORR_SIZE]();

    for (int lag = 0; lag < XCORR_SIZE; lag++)
    {
        for (int i = 0; i < XCORR_SIZE; i++)
        {
            int j = i - lag + XCORR_SIZE - 1;

            // The symbol and sync buffers are the same size so no
            // bounds check is needed here (e.g., autocorrelation)
            res[lag] += buff[i] * m_SyncTab[j];

            if (res[lag] < min)
                min = res[lag];
            else if (res[lag] > max)
                max = res[lag];
        }
    }

    delete[] res;
}
