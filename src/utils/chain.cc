// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "chain.h"

using namespace util;

void chain::add(dsp::block<dsp::func_ff> &block, const char *name)
{
    m_Chain.push_back(link { ff, &block, name, block.getType() });
}

void chain::add(std::unique_ptr<dsp::block<dsp::func_ff>> &&block, const char *name)
{
    m_Chain.push_back(link { ff, block.get(), name, block->getType() });
    m_FloatBlocks.push_back(std::move(block));
}

void chain::add(dsp::block<dsp::func_fc> &block, const char *name)
{
    m_Chain.push_back(link { fc, &block, name, block.getType() });
}

void chain::add(std::unique_ptr<dsp::block<dsp::func_fc>> &&block, const char *name)
{
    m_Chain.push_back(link { fc, block.get(), name, block->getType() });
    m_FloatCmplxBlocks.push_back(std::move(block));
}

void chain::add(dsp::block<dsp::func_cf> &block, const char *name)
{
    m_Chain.push_back(link { cf, &block, name, block.getType() });
}

void chain::add(std::unique_ptr<dsp::block<dsp::func_cf>> &&block, const char *name)
{
    m_Chain.push_back(link { cf, block.get(), name, block->getType() });
    m_CmplxFloatBlocks.push_back(std::move(block));
}

void chain::add(dsp::block<dsp::func_cc> &block, const char *name)
{
    m_Chain.push_back(link { cc, &block, name, block.getType() });
}

void chain::add(std::unique_ptr<dsp::block<dsp::func_cc>> &&block, const char *name)
{
    m_Chain.push_back(link { cc, block.get(), name, block->getType() });
    m_CmplxBlocks.push_back(std::move(block));
}

bool chain::setup(void)
{
    assert(m_Chain.size() > 1);

    m_Trace.print(ID, "Checking chain %s [sz=%u]\n", m_Name, static_cast<uint32_t>(m_Chain.size()));

    for (size_t i=0; i < (m_Chain.size() - 1);i++)
    {
        m_Trace.print(ID, "Checking link %s -> %s\n", m_Chain[i].name, m_Chain[i+1].name);

        if (!isValidLink(m_Chain[i].iface, m_Chain[i+1].iface))
        {
            m_Trace.print(ID, "Invalid link! I/O doesn't match. ABORT\n");
            return false;
        }
    }

    m_Trace.print(ID, "All links are valid. Checking chain endpoints...\n");

    if ((m_Chain[0].type != dsp::TYPE_BIDIR) || (m_Chain[m_Chain.size() - 1].type != dsp::TYPE_BIDIR))
    {
        m_Trace.print(ID, "Is the first link a source?\n");
        if ((m_Chain[0].type != dsp::TYPE_SOURCE) && (m_Chain[0].type != dsp::TYPE_BIDIR))
        {
            m_Trace.print(ID, "NO -- FAIL\n");
            return false;
        }

        m_Trace.print(ID, "YES. Is the last link a sink?\n");

        if ((m_Chain[m_Chain.size() - 1].type != dsp::TYPE_SINK) && (m_Chain[m_Chain.size() - 1].type != dsp::TYPE_BIDIR))
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

    if (m_Chain[0].type == dsp::TYPE_BIDIR)
        m_Chain[0].bimode = dsp::BIDIR_SOURCE;

    if (m_Chain[m_Chain.size() - 1].type == dsp::TYPE_BIDIR)
        m_Chain[m_Chain.size() - 1].bimode = dsp::BIDIR_SINK;

    m_IsChecked = true;

    m_Trace.print(ID, "%s is all good\n", m_Name);
    return true;
}

void chain::iterate()
{
    assert(m_IsChecked);

    dsp::rate_t rate = 0;

    util::aligned_ptr<float> *pFloatIn = nullptr;
    util::aligned_ptr<float> *pFloatOut = nullptr;

    util::aligned_ptr<rm_math::complex_f> *pCmplxIn = nullptr;
    util::aligned_ptr<rm_math::complex_f> *pCmplxOut = nullptr;

    for (size_t i=0; i < m_Chain.size();i++)
    {
        switch(m_Chain[i].iface)
        {
            case ff:
                m_LinkTrace.print(ID, "F -> F\n");

                pFloatIn = &m_fBuff[m_FIdx];
                m_FIdx = (m_FIdx + 1) & 1;
                pFloatOut = &m_fBuff[m_FIdx];
                handleLink<dsp::func_ff, float, float>(m_Chain[i], rate, *pFloatIn, *pFloatOut);
                break;

            case fc:
                m_LinkTrace.print(ID, "F -> C\n");

                pFloatIn = &m_fBuff[m_FIdx];
                pCmplxOut = &m_cBuff[m_CIdx];
                handleLink<dsp::func_fc, float, rm_math::complex_f>(m_Chain[i], rate, *pFloatIn, *pCmplxOut);
                break;

            case cf:
                m_LinkTrace.print(ID, "C -> F\n");

                pCmplxIn = &m_cBuff[m_CIdx];
                pFloatOut = &m_fBuff[m_FIdx];
                handleLink<dsp::func_cf, rm_math::complex_f, float>(m_Chain[i], rate, *pCmplxIn, *pFloatOut);
                break;

            case cc:
                m_LinkTrace.print(ID, "C -> C\n");

                pCmplxIn = &m_cBuff[m_CIdx];
                m_CIdx = (m_CIdx + 1) & 1;
                pCmplxOut = &m_cBuff[m_CIdx];
                handleLink<dsp::func_cc, rm_math::complex_f, rm_math::complex_f>(m_Chain[i], rate, *pCmplxIn, *pCmplxOut);
                break;

            default:
                assert(1);
        }
    }
}

void chain::clear()
{
    m_Chain.clear();

    m_FloatBlocks.clear();
    m_FloatCmplxBlocks.clear();
    m_CmplxFloatBlocks.clear();
    m_CmplxBlocks.clear();

    m_fBuff[0].clear();
    m_fBuff[1].clear();
    m_cBuff[0].clear();
    m_cBuff[1].clear();

    m_IsChecked = false;
}
