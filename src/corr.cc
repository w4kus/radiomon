#include "corr.h"
#include "log.h"
#include <cstring>

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
               m_SymbolRingBuff(SYMBOL_Q_SIZE),
               m_SymBuffP(nullptr),
               m_SymBuffBurstStart(nullptr),
               m_SymBuffEnd(nullptr),
               m_SymBuffCount(0),
               m_IsCorrelated(false)
{

    // These can throw an exception
    buildSymTable();
    std::thread(&corr::handleSymbols, this).detach();
}

corr::corr(mode_t mode) : m_Running(true),
               m_Mode(mode),
               m_SymbolRingBuff(SYMBOL_Q_SIZE),
               m_SymBuffP(nullptr),
               m_SymBuffBurstStart(nullptr),
               m_SymBuffEnd(nullptr),
               m_SymBuffCount(0),
               m_IsCorrelated(false)
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
    // generate the sync symbol table in reverse order for correlation
    int dstIdx = 0, srcIdx = SYNC_WORD_SYMBOL_NUM - 1;

    symbol_t *srcTab = nullptr;

    switch(m_Mode)
    {
        case MODE_BS:
            srcTab = (symbol_t *)sym_table_bs_source_data;
            break;

        default:
            throw error("Mode is not supported right now");
    }

    std::FILE *f = std::fopen("test_sync.txt", "w");
    for (int i=0;i < SYNC_WORD_SYMBOL_NUM;i++)
    {
        for (int j=0;j < SAMPLES_PER_SYMBOL;j++)
        {
            m_SyncTab[dstIdx++] = srcTab[srcIdx];
            std::fprintf(f, "%.04f,", srcTab[srcIdx]);
        }
        
        --srcIdx;
    }

    std::fclose(f);

    m_SymBuffP = m_SymBuff;
    m_SymBuffEnd = &m_SymBuff[SYMBUFF_SIZE];
}

////////////////////////////////////////////////////////////////////////////////////
// Thread context
void corr::handleSymbols(corr *inst)
{
    log::getInst()->trace(log::CORR, "searching...\n");

    while(inst->m_Running)
    {
        if (inst->m_SymbolRingBuff.pop(*(inst->m_SymBuffP)))
        {
            ++inst->m_SymBuffCount;

            if (inst->m_SymBuffCount >= SYNC_WORD_SIZE)
            {
                if (!inst->m_IsCorrelated)
                {
                    if (inst->correlate())
                    {
                        log::getInst()->trace(log::CORR, "Correlated!\n");
                        inst->m_IsCorrelated = true;
                    }
                }
                else
                {
                    // accumulate the remainder of the data burst
                    if (inst->accum())
                    {
                        // Send burst to decoder and continue
                        inst->decode();
                        log::getInst()->trace(log::CORR, "Searching...\n");
                        inst->m_IsCorrelated = false;
                    }
                }
            }

            inst->m_SymBuffCount = inst->next();
        }
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

bool corr::correlate()
{
    // Build the correlation buffer

    return false;
}

bool corr::accum()
{
    return true;
}

void corr::decode()
{

}

size_t corr::next(size_t n)
{
    return 0;
}

#if 0
#endif

// TEMP DEBUG
void corr::test()
{
    int dstIdx = 0, srcIdx = 0;
    std::FILE *f = std::fopen("test-corr.txt", "w");
    symbol_t *srcTab = (symbol_t *)sym_table_bs_source_data;

    log::log::getInst()->trace(log::CORR, "Starting test\n");

    for (int i=0;i < SYNC_WORD_SYMBOL_NUM;i++)
    {
        for (int j=0;j < SAMPLES_PER_SYMBOL;j++)
        {
            std::fprintf(f, "%.04f,", srcTab[srcIdx]);
            m_CorrBuff[dstIdx++] = srcTab[srcIdx];
        }

        ++srcIdx;
    }

    std::fclose(f);

    directConvolve();
}

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))
int corr::directConvolve()
{
    int i, j, corr_start, sync_start, sync_end;

    // symbol_t result;
    memset(testBuff, 0, sizeof(testBuff));
    std::FILE *f = std::fopen("conv-test.txt", "w");

    for (i=0; i < CONVOLVE_SIZE; i++)
    {
        sync_start = MAX(0, i - SYNC_WORD_SIZE + 1);
        sync_end   = MIN(i + 1, SYNC_WORD_SIZE);
        corr_start = MIN(i, SYNC_WORD_SIZE - 1);

        for(j = sync_start; j < sync_end; j++)
            testBuff[i] += m_CorrBuff[corr_start--] * m_SyncTab[j];

        std::fprintf(f, "%.04f,", testBuff[i]);
    }

    std::fprintf(f, "\n");
    std::fclose(f);

    return 0;
}

#if 0
int corr::directConvolve()
{
    int i, j, diff;

    // symbol_t result;
    memset(testBuff, 0, sizeof(testBuff));
    std::FILE *f = std::fopen("conv-test.txt", "w");

    for (i=0;i < CONVOLVE_SIZE;i++)
    {
        testBuff[i] = 0;

        for (j=0;j < SYNC_WORD_SIZE;j++)
        {
            diff = i - j;

            if ((diff >= 0) && (diff <= SYNC_WORD_SIZE))
                testBuff[i] += m_CorrBuff[j] * m_SyncTab[diff];
        }

        std::fprintf(f, "%.04f,", testBuff[i]);
    }

    std::fprintf(f, "\n");
    std::fclose(f);

    return 0;
}

void convolve(const double Signal[/* SignalLen */], size_t SignalLen,
              const double Kernel[/* KernelLen */], size_t KernelLen,
              double Result[/* SignalLen + KernelLen - 1 */])
{
  size_t n;

  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    size_t kmin, kmax, k;

    Result[n] = 0;

    kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
    kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

    for (k = kmin; k <= kmax; k++)
    {
      Result[n] += Signal[k] * Kernel[n - k];
    }
  }
}
float* convolve(float h[], float x[], int lenH, int lenX, int* lenY)
{
  int nconv = lenH+lenX-1;
  (*lenY) = nconv;
  int i,j,h_start,x_start,x_end;

  float *y = (float*) calloc(nconv, sizeof(float));

  for (i=0; i<nconv; i++)
  {
    x_start = MAX(0,i-lenH+1);
    x_end   = MIN(i+1,lenX);
    h_start = MIN(i,lenH-1);
    for(j=x_start; j<x_end; j++)
    {
      y[i] += h[h_start--]*x[j];
    }
  }
  return y;
}
#endif

int corr::fftConvolve()
{
    return 0;
}
