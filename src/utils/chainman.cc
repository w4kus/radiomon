// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "chainman.h"

using namespace util;

static constexpr uint8_t IDX_FF = 0;
static constexpr uint8_t IDX_FC = 1;
static constexpr uint8_t IDX_CF = 2;
static constexpr uint8_t IDX_CC = 3;

static const std::array<size_t, 4> hashCodes =
{
    typeid(dsp::func_ff).hash_code(),   // IDX_FF
    typeid(dsp::func_fc).hash_code(),   // IDX_FC
    typeid(dsp::func_cf).hash_code(),   // IDX_CF
    typeid(dsp::func_cc).hash_code()    // IDX_CC
};

void chainman::add(dsp::block<dsp::func_ff> &block, const char *name)
{
    m_Chain.push_back(  link {
                            typeid(dsp::func_ff).hash_code(), &block, name, block.getType(),
                            link::INPUT_FLOAT | link::OUTPUT_FLOAT
                        }
                    );
}

void chainman::add(const std::shared_ptr<dsp::block<dsp::func_ff>> block, const char *name)
{
    m_Chain.push_back(  link {
                            typeid(dsp::func_fc).hash_code(), block.get(), name, block->getType(),
                            link::INPUT_FLOAT | link::OUTPUT_FLOAT
                        }
                    );

    // Assume we are the owner now.
    m_FloatBlocks.push_back(std::move(block));
}

void chainman::add(dsp::block<dsp::func_fc> &block, const char *name)
{
    m_Chain.push_back(  link {
                            typeid(dsp::func_fc).hash_code(), &block, name, block.getType(),
                            link::INPUT_FLOAT | link::OUTPUT_CMPLX
                        }
                    );
}

void chainman::add(const std::shared_ptr<dsp::block<dsp::func_fc>> block, const char *name)
{
    m_Chain.push_back(  link {
                            typeid(dsp::func_fc).hash_code(), block.get(), name, block->getType(),
                            link::INPUT_FLOAT | link::OUTPUT_CMPLX
                        }
                    );

    // Assume we are the owner now.
    m_FloatCmplxBlocks.push_back(std::move(block));
}

void chainman::add(dsp::block<dsp::func_cf> &block, const char *name)
{
    m_Chain.push_back(  link {
                            typeid(dsp::func_cf).hash_code(), &block, name, block.getType(),
                            link::INPUT_CMPLX | link::OUTPUT_FLOAT
                        }
                    );
}

void chainman::add(const std::shared_ptr<dsp::block<dsp::func_cf>> block, const char *name)
{
    m_Chain.push_back(  link {
                            typeid(dsp::func_cf).hash_code(), block.get(), name, block->getType(),
                            link::INPUT_CMPLX | link::OUTPUT_FLOAT
                        }
                    );

    // Assume we are the owner now.
    m_CmplxFloatBlocks.push_back(std::move(block));
}

void chainman::add(dsp::block<dsp::func_cc> &block, const char *name)
{
    m_Chain.push_back(  link {
                            typeid(dsp::func_cc).hash_code(), &block, name, block.getType(),
                            link::INPUT_CMPLX | link::OUTPUT_CMPLX
                        }
                    );
}

void chainman::add(const std::shared_ptr<dsp::block<dsp::func_cc>> block, const char *name)
{
    m_Chain.push_back(  link {
                            typeid(dsp::func_cc).hash_code(), block.get(), name, block->getType(),
                            link::INPUT_CMPLX | link::OUTPUT_CMPLX
                        }
                    );

    // Assume we are the owner now.
    m_CmplxBlocks.push_back(std::move(block));
}

bool chainman::check(void)
{
    assert(m_Chain.size() > 1);

    m_Trace.print(ID, "Checking chain %s [sz=%u]\n", m_Name, static_cast<uint32_t>(m_Chain.size()));

    for (size_t i=0; i < (m_Chain.size() - 1);i++)
    {
        auto lnk1 = m_Chain[i];
        auto lnk2 = m_Chain[i+1];

        m_Trace.print(ID, "Checking links %s -> %s\n", m_Chain[i].name, m_Chain[i+1].name);

        for (size_t j=0; j < hashCodes.size();j++)
        {
            if (lnk1.hash == hashCodes[j])
            {
                if (!isValidLink(lnk1.iomap, lnk2.iomap))
                {
                    m_Trace.print(ID, "Invalid link! I/O doesn't match. ABORT\n");
                    return false;
                }
            }
        }
    }

    m_Trace.print(ID, "All links are valid. Checking chain endpoints...\n");

    if ((m_Chain[0].type != dsp::TYPE_ENDPOINT) || (m_Chain[m_Chain.size() - 1].type != dsp::TYPE_ENDPOINT))
    {
        m_Trace.print(ID, "Is the first link a source?\n");
        if ((m_Chain[0].type != dsp::TYPE_SOURCE) && (m_Chain[0].type != dsp::TYPE_ENDPOINT))
        {
            m_Trace.print(ID, "NO -- FAIL\n");
            return false;
        }

        m_Trace.print(ID, "YES. Is the last link a sink?\n");

        if ((m_Chain[m_Chain.size() - 1].type != dsp::TYPE_SINK) && (m_Chain[m_Chain.size() - 1].type != dsp::TYPE_ENDPOINT))
        {
            m_Trace.print(ID, "NO -- FAIL\n");
            return false;
        }

        m_Trace.print(ID, "YES\n");
    }
    else
    {
        m_Trace.print(ID, "First and last links are endpoints -- GOOD\n");
    }

    m_Trace.print(ID, "%s is all good\n", m_Name);
    return true;
}

void chainman::iterate()
{
    for (size_t i=0; i < m_Chain.size();i++)
    {
        if (m_Chain[i].hash == hashCodes[m_Chain[IDX_FF].iomap])
        {
            auto blk = (dsp::block<dsp::func_ff> *)m_Chain[i].block;

            m_fBuff[IN_IDX] = std::move(m_fBuff[OUT_IDX]);
            blk->getProcesser()(m_fBuff[IN_IDX], m_fBuff[OUT_IDX]);
        }
        else if (m_Chain[i].hash == hashCodes[m_Chain[IDX_FC].iomap])
        {
            auto blk = (dsp::block<dsp::func_fc> *)m_Chain[i].block;

            m_fBuff[IN_IDX] = std::move(m_fBuff[OUT_IDX]);
            blk->getProcesser()(m_fBuff[IN_IDX], m_cBuff[OUT_IDX]);
        }
        else if (m_Chain[i].hash == hashCodes[m_Chain[IDX_CF].iomap])
        {
            auto blk = (dsp::block<dsp::func_cf> *)m_Chain[i].block;

            m_cBuff[IN_IDX] = std::move(m_cBuff[OUT_IDX]);
            blk->getProcesser()(m_cBuff[IN_IDX], m_fBuff[OUT_IDX]);
        }
        else if (m_Chain[i].hash == hashCodes[m_Chain[IDX_CC].iomap])
        {
            auto blk = (dsp::block<dsp::func_cc> *)m_Chain[i].block;

            m_cBuff[IN_IDX] = std::move(m_cBuff[OUT_IDX]);
            blk->getProcesser()(m_cBuff[IN_IDX], m_cBuff[OUT_IDX]);
        }
    }
}

void chainman::clear()
{
    m_Chain.clear();

    m_FloatBlocks.clear();
    m_FloatCmplxBlocks.clear();
    m_CmplxFloatBlocks.clear();
    m_CmplxBlocks.clear();

    m_fBuff[IN_IDX].clear();
    m_fBuff[OUT_IDX].clear();
    m_cBuff[IN_IDX].clear();
    m_cBuff[OUT_IDX].clear();
}
