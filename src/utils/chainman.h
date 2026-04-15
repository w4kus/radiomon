// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <vector>
#include <typeinfo>
#include <memory>

#include "block.h"
#include "trace.h"
#include "aligned-ptr.h"

namespace util {

/*! \brief Chain Manager
 *
 *
*/

class chainman
{
public:

    chainman(const char *name) : m_Name(name)
    {
    }

    chainman() = default;

    chainman(const chainman &) = delete;
    chainman& operator=(const chainman &) = delete;

    chainman(const chainman &&) = delete;
    chainman&& operator=(chainman &&) = delete;

    void add(dsp::block<dsp::func_ff> &block, const char *name);
    void add(const std::shared_ptr<dsp::block<dsp::func_ff>> block, const char *name);

    void add(dsp::block<dsp::func_fc> &block, const char *name);
    void add(const std::shared_ptr<dsp::block<dsp::func_fc>> block, const char *name);

    void add(dsp::block<dsp::func_cf> &block, const char *name);
    void add(const std::shared_ptr<dsp::block<dsp::func_cf>> block, const char *name);

    void add(dsp::block<dsp::func_cc> &block, const char *name);
    void add(const std::shared_ptr<dsp::block<dsp::func_cc>> block, const char *name);

    bool check();
    void iterate();
    void clear();

protected:

    struct link
    {
        link(const size_t h, const void *b, const char *n, const dsp::block_type t, const uint8_t io) :
            hash { h }, block { b }, name { n }, iomap { io }, type { t }
        {
        }

        const size_t hash;
        const void *block;
        const char *name;

        // bit 0 -- block input type    -- 0=float type, 1=complex type
        // bit 1 -- block output type   -- 0=float type, 1=complex type
        // Note that the bit combinations map to the hash code table and is used for lookups.
        static constexpr uint8_t INPUT_FLOAT    = 0;
        static constexpr uint8_t INPUT_CMPLX    = 1;
        static constexpr uint8_t OUTPUT_FLOAT   = 0;
        static constexpr uint8_t OUTPUT_CMPLX   = 2;
        const uint8_t iomap;

        const dsp::block_type type;
    };

    std::vector<link> m_Chain;

    static constexpr uint8_t IN_IDX         = 0;
    static constexpr uint8_t OUT_IDX        = 1;

    util::aligned_ptr<float>                m_fBuff[2];
    util::aligned_ptr<rm_math::complex_f>   m_cBuff[2];

    std::vector<std::shared_ptr<dsp::block<dsp::func_ff>>> m_FloatBlocks;
    std::vector<std::shared_ptr<dsp::block<dsp::func_fc>>> m_FloatCmplxBlocks;
    std::vector<std::shared_ptr<dsp::block<dsp::func_cf>>> m_CmplxFloatBlocks;
    std::vector<std::shared_ptr<dsp::block<dsp::func_cc>>> m_CmplxBlocks;

    bool isValidLink(uint8_t input, uint8_t output)
    {
        return !((output >> 1) ^ input);
    }

    const char *m_Name;

    static constexpr char const *ID = "CHAIN";
    util::trace<true> m_Trace;
};

}
