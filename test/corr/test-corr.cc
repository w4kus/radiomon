#include <stdio.h>
#include <cstring>
#include <thread>
#include "corr.h"
#include "cmdline.h"
#include "log.h"

#include <semaphore.h>

namespace utils = dmr::util;
using namespace dmr;

const int SYNC_SIZE     = dmr::SYNC_WORD_SYMBOL_NUM * dmr::SAMPLES_PER_SYMBOL; 
const int SIG_SIZE_MIN  = SYNC_SIZE;

static void insertSync(dmr::symbol_t *buff, bool compliment);
static void insertPayload(dmr::symbol_t *buff);
static void insertCach(dmr::symbol_t *buff);

static symbol_t nextNoiseChar();
static void buildTestSig(int numFrames, int padSymNum, bool compliment);

static void test_detect_voice(int numFrames, int padSymNum);
static void test_detect_data(int numFrames, int padSymNum);
static void test_wait(int size);

static void test_callback(int corrRes, uint8_t *decRes);

static symbol_t *testSig = nullptr;
static int testSigSize;

static corr testCorrelator(dmr::corr::MODE_BS, (corr::corr_callback_t)test_callback, true);
static uint8_t noiseCharIdx;

static sem_t sem;

int main(int argc, char **argv)
{
    std::string options;
    int numFrames = 2;
    int padSymNum = 32;

    log::getInst()->SetLogActiveMods(log::CORR);

    float sum = 0;
    for (int i=0;i < SYNC_WORD_SYMBOL_NUM;i++)
        sum += sym_table_bs_source_voice[i];

    printf("BS voice sum=%.02f\n", sum);

    sum = 0;
    for (int i=0;i < SYNC_WORD_SYMBOL_NUM;i++)
        sum += sym_table_ms_source_voice[i];

    printf("MS voice sum=%.02f\n", sum);

    sum = 0;
    for (int i=0;i < SYNC_WORD_SYMBOL_NUM;i++)
        sum += sym_table_direct_ts1_voice[i];

    printf("TS1 voice sum=%.02f\n", sum);

    sum = 0;
    for (int i=0;i < SYNC_WORD_SYMBOL_NUM;i++)
        sum += sym_table_direct_ts2_voice[i];

    printf("TS2 voice sum=%.02f\n", sum);

    auto str = utils::getCmdOption(argv, argv + argc, "-n");

    if (str)
        numFrames = std::strtol(str, NULL, 10);
    
    printf("Number of frames: %d\n", numFrames);

    str = utils::getCmdOption(argv, argv + argc, "-p");

    if (str)
        padSymNum = std::strtol(str, NULL, 10);

    testSigSize = (numFrames * (dmr::BURST_FRAME_SYMBOL_NUM * dmr::SAMPLES_PER_SYMBOL)) + (padSymNum * dmr::SAMPLES_PER_SYMBOL);
    testSig = new dmr::symbol_t[testSigSize]();

    printf("TESTCORR: using %d symbol test signal with %d frames"
            " and %d symbols of padding\n", testSigSize, numFrames, padSymNum);

    sem_init(&sem, 0, 1);

    test_detect_voice(numFrames, padSymNum);
    test_detect_data(numFrames, padSymNum);

    delete[] testSig;
    return 0;
}

const symbol_t noiseChars[32] =
{
    -1, -0.6, 1.2, 3.5, 2.1, -4.5, -0.3, 0.3, -0.32, -1,
    -1, -0.6, 0.2, -4.3, 2.6, -3.5, 1.3, 0.3, 0.66, -1.1,
    -1, 0.6, 0.2, 2.8, -2.1, -0.5, -0.73, -0.3, -0.32, 1,
    -0.88, 1.15
};

const symbol_t payload[dmr::BURST_PAYLOAD_SECT_SYMBOL_NUM] =
{
    -1, -0.3, 0.3, 1, 0.3, -0.3, -1, 1, -1, 1,
    -1, -0.3, 0.3, 1, 0.3, -0.3, -1, 1, -1, 1,
    -1, -0.3, 0.3, 1, 0.3, -0.3, -1, 1, -1, 1,
    -1, -0.3, 0.3, 1, 0.3, -0.3, -1, 1, -1, 1,
    -1, -0.3, 0.3, 1, 0.3, -0.3, -1, 1, -1, 1,
    -1, -0.3, 0.3, 1
};

const symbol_t cach[dmr::TDMA_CACH_SYMBOL_NUM] =
{
    -1, -1 , -1, -1, -1, -1, 1, 1, 1, 1, 1, 1
};

static void insertSync(dmr::symbol_t *buff, bool compliment)
{
    dmr::symbol_t *p = buff;

    for (int i=0; i < dmr::SYNC_WORD_SYMBOL_NUM;i++)
    {
        for (int j=0; j < dmr::SAMPLES_PER_SYMBOL;j++)
            *p++ = (!compliment) ? sym_table_bs_source_voice[i] : -sym_table_bs_source_voice[i];
    }
}

static void insertPayload(dmr::symbol_t *buff)
{
    dmr::symbol_t *p = buff;

    for (int i=0; i < dmr::BURST_PAYLOAD_SECT_SYMBOL_NUM;i++)
    {
        for (int j=0; j < dmr::SAMPLES_PER_SYMBOL;j++)
            *p++ = payload[i];
    }
}

static void insertCach(dmr::symbol_t *buff)
{
    dmr::symbol_t *p = buff;

    for (int i=0; i < dmr::TDMA_CACH_SYMBOL_NUM;i++)
    {
        for (int j=0; j < dmr::SAMPLES_PER_SYMBOL;j++)
            *p++ = cach[i];
    }
}

static symbol_t nextNoiseChar()
{
    auto ch = noiseChars[noiseCharIdx];

    ++noiseCharIdx;
    noiseCharIdx &= 0x1F;

    return ch;
}

static void buildTestSig(int numFrames, int padSymNum, bool compliment)
{
    int idx = 0;

    while(numFrames)
    {
        insertPayload(&testSig[idx]);
        idx += dmr::BURST_PAYLOAD_SECT_SYMBOL_NUM * dmr::SAMPLES_PER_SYMBOL;

        insertSync(&testSig[idx], compliment);
        idx += dmr::SYNC_WORD_SYMBOL_NUM * dmr::SAMPLES_PER_SYMBOL;

        insertPayload(&testSig[idx]);
        idx += dmr::BURST_PAYLOAD_SECT_SYMBOL_NUM * dmr::SAMPLES_PER_SYMBOL;

        insertCach(&testSig[idx]);
        idx += dmr::TDMA_CACH_SYMBOL_NUM * dmr::SAMPLES_PER_SYMBOL;

        --numFrames;
    }

    for (int i=0;i < padSymNum;i++)
    {
        for (int j=0;j < dmr::SAMPLES_PER_SYMBOL;j++)
            testSig[idx++] = nextNoiseChar();
    }
}

static void test_detect_voice(int numFrames, int padSymNum)
{
    printf("TESTCORR: testing voice sync\n");
    buildTestSig(numFrames, padSymNum, false);
    test_wait(numFrames);
}

static void test_detect_data(int numFrames, int padSymNum)
{
    printf("TESTCORR: testing data sync\n");
    buildTestSig(numFrames, padSymNum, true);
    test_wait(numFrames);
}

static void test_wait(int numFrames)
{
    // If semaphore is posted, put it into wait
    int val = 0;
    sem_getvalue(&sem, &val);

    if (val)
        sem_wait(&sem);

    testCorrelator.pushSymbols(testSigSize, testSig);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // wait for post
    sem_wait(&sem);
}

static void test_callback(int corrRes, uint8_t *decRes)
{
    static int count = 0;

    ++count;
    printf("TESTCORR: %d syncs found\n", count);

    sem_post(&sem);
}