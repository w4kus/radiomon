#include <pthread.h>
#include "corr.h"

// BS source sync: data (compliment: voice)
//   D      F      F      5     7      D      7      5      D      F      5       D
// -3 +3 -3 -3  -3 -3  +3 +3  +3 -3  -3 +3  +3 -3  +3 +3  -3 +3  -3 -3  +3 +3  -3 +3
//
// Normalized data sync symbol table
const dmr::symbol_t sym_table_bs_source_data[] = 
{ 
    -1, +1, -1, -1,     // DF
    -1, -1, +1, +1,     // F5
    +1, -1, -1, +1,     // 7D
    +1, -1, +1, +1,     // 75
    -1, +1, -1, -1,     // DF
    +1, +1, -1, +1      // 5D
};

using namespace dmr;

corr::corr() : m_Running(true),
               m_Mode(MODE_BS),
               m_SymbolQ(SYMBOL_Q_SIZE),
               m_SymBuffP(nullptr),
               m_SymBuffBurstStart(nullptr),
               m_SymBuffEnd(nullptr)
{

    // These can throw an exception
    buildSymTable();
    std::thread(&corr::handleSymbols, this).detach();
}

corr::corr(mode_t mode) : m_Running(true),
               m_Mode(mode),
               m_SymbolQ(SYMBOL_Q_SIZE),
               m_SymBuffP(nullptr),
               m_SymBuffBurstStart(nullptr),
               m_SymBuffEnd(nullptr)
{
    // These can throw an exception
    buildSymTable();
    std::thread(&corr::handleSymbols, this).detach();
}

corr::~corr()
{
    m_Running = false;
}

void corr::buildSymTable()
{
    // generate the symbol table
    int dstIdx = 0, srcIdx = 0;

    symbol_t *srcTab = nullptr;

    switch(m_Mode)
    {
        case MODE_BS:
            srcTab = (symbol_t *)sym_table_bs_source_data;
            break;

        default:
            throw error("Mode is not supported right now");
    }

    for (int i=0;i < SYNC_WORD_SYMBOL_NUM;i++)
    {
        for (int j=0;j < SAMPLES_PER_SYMBOL;j++)
            m_SyncTab[dstIdx++] = srcTab[srcIdx];
        
        ++srcIdx;
    }

    m_SymBuffP = m_SymBuff;
    m_SymBuffEnd = &m_SymBuff[SYMBUFF_SIZE];
}

////////////////////////////////////////////////////////////////////////////////////
// Thread context
void corr::handleSymbols(corr *inst)
{
    while(inst->m_Running)
    {   
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
