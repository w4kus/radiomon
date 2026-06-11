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

/*! \brief Create a Chain of DSP Blocks
 *
 * A *chain* is a sequence of blocks in which data is passed from one end
 * of the chain to the other end. The data originates from a *source* block (e.g., a driver
 * for an SDR) and gets passed through each block in the chain until it reaches the *sink*
 * block at the end. In the context of a chain, each block is a *link* and there are
 * currently five types of links:
 *
 * * **Operator**  Most links are of this type. They operate on each block of data before
 *     passing it on to the next link.
 * * **Source**  This link provides the data to the chain.
 * * **Sink**  This link takes the result of the chain and passes to some receiver, e.g., a
 *     radio app.
 * * **BIDIR**  A combined source and sink. This type of link can be at both ends of the chain.
 * * **Resampler**  This is an operator which changes the sampling rate, e.g., a
 *     rational resampler. These links are read by the chain logic to update the sampling rate which
 *     is then given to all links that follow.
 *
 * An example of a chain would be, say, an FSK demodulator.
 *
 * \note The constructors can be divided into two types based on how blocks are passed in:
 * * **References** where a reference to a block is passed in. This requires that the caller ensure
 *   that the block instance remains active while the chain is in use. The also allows you to make any
 *   runtime modifications to the block if desired and the block supports it.
 * * **Pointers** where a *std::unique_ptr* containing the block is passed in. This is the
 *   *add-and-forget* method which gives ownership of the pointer to the chain instance. It will be
 *   destroyed when the chain is destroyed.
*/

class chain
{
public:

    //! Create an instance of a chain. The name is set to a default value.
    chain() : m_IsChecked { false }, m_FIdx { 0 }, m_CIdx { 0 }
    {
        m_Name = "THE_CHAIN";
    }

    //! Create an instance of a chain.
    //! @param [in] name  The name of the chain. This is for the benefit of the developer.
    chain(const char *name) : m_Name { name }, m_IsChecked { false }, m_FIdx { 0 }, m_CIdx { 0 }
    {
    }

    chain(const chain &) = delete;
    chain& operator=(const chain &) = delete;

    chain(chain &&) = delete;
    chain& operator=(chain &&) = delete;

    //! Add a block which takes floating point data and outputs floating point data.
    //! @param [in] block  A reference to the block
    //! @param [in] name   The name of the block. This is for the benefit of the developer.
    //! \warning The caller is responsible for making sure the lifetime of the block is
    //! maintained while the chain is active.
    void add(dsp::block<dsp::func_ff> &block, const char *name);

    //! Add a block which takes floating point data and outputs floating point data.
    //! @param [in] block  A unique_ptr which contains the block to be added.
    //! @param [in] name   The name of the block. This is for the benefit of the developer.
    //! \note *block* is no longer valid after calling this method.
    void add(std::unique_ptr<dsp::block<dsp::func_ff>> &&block, const char *name);

    //! Add a block which takes floating point data and outputs complex data.
    //! @param [in] block  A reference to the block
    //! @param [in] name   The name of the block. This is for the benefit of the developer.
    //! \warning The caller is responsible for making sure the lifetime of the block is
    //! maintained while the chain is active.
    void add(dsp::block<dsp::func_fc> &block, const char *name);

    //! Add a block which takes floating point data and outputs complex data.
    //! @param [in] block  A unique_ptr which contains the block to be added.
    //! @param [in] name   The name of the block. This is for the benefit of the developer.
    //! \note *block* is no longer valid after calling this method.
    void add(std::unique_ptr<dsp::block<dsp::func_fc>> &&block, const char *name);

    //! Add a block which takes complex data and outputs floating point data.
    //! @param [in] block  A reference to the block
    //! @param [in] name   The name of the block. This is for the benefit of the developer.
    //! \warning The caller is responsible for making sure the lifetime of the block is
    //! maintained while the chain is active.
    void add(dsp::block<dsp::func_cf> &block, const char *name);

    //! Add a block which takes complex data and outputs floating point data.
    //! @param [in] block  A unique_ptr which contains the block to be added.
    //! @param [in] name   The name of the block. This is for the benefit of the developer.
    //! \note *block* is no longer valid after calling this method.
    void add(std::unique_ptr<dsp::block<dsp::func_cf>> &&block, const char *name);

    //! Add a block which takes complex data and outputs complex data.
    //! @param [in] block  A reference to the block
    //! @param [in] name   The name of the block. This is for the benefit of the developer.
    //! \warning The caller is responsible for making sure the lifetime of the block is
    //! maintained while the chain is active.
    void add(dsp::block<dsp::func_cc> &block, const char *name);

    //! Add a block which takes complex data and outputs complex data.
    //! @param [in] block  A unique_ptr which contains the block to be added.
    //! @param [in] name   The name of the block. This is for the benefit of the developer.
    //! \note *block* is no longer valid after calling this method.
    void add(std::unique_ptr<dsp::block<dsp::func_cc>> &&block, const char *name);

    //! Once all the blocks of a chain have been added, this routine must be called to validate
    //! the links and complete the setup. A failure indicates a problem and it should be fixed
    //! before iteration.
    //! @return **true** if the chain is valid, **false** otherwise.
    bool setup();

    //! Calling this method will iterate through the chain once. Where this is called and how
    //! often its called is up to the application. For a single threaded app or on a threadless platform,
    //! you would probably call this in a loop and potentially process the results after each call. A multithreaded
    //! app might place this in a high priority thread with the results being passed to another
    //! thread via a ring buffer. It really all depends on how much data the chain's source
    //! generates.
    //! \note Always be mindful of overruns in the source and underruns in the sink.
    void iterate();

    //! This resets the chain to a cleared state with all allocated objects freed up.
    //! \note You do not have to call this before destruction; this may useful to
    //! an application that needs to rebuild a chain for some reason.
    void clear();

protected:

    //! \cond

    // bit 0 -- block input type    -- 0=float type, 1=complex type
    // bit 1 -- block output type   -- 0=float type, 1=complex type
    static constexpr uint8_t INPUT_FLOAT    = 0;
    static constexpr uint8_t INPUT_CMPLX    = 1;
    static constexpr uint8_t OUTPUT_FLOAT   = 0;
    static constexpr uint8_t OUTPUT_CMPLX   = 2;

    enum interface
    {
        ff = INPUT_FLOAT | OUTPUT_FLOAT,
        cf = INPUT_CMPLX | OUTPUT_FLOAT,
        fc = INPUT_FLOAT | OUTPUT_CMPLX,
        cc = INPUT_CMPLX | OUTPUT_CMPLX
    };

    struct link
    {
        link(const interface i, void *b, const char *n, const dsp::block_type t) :
            iface { i }, block { b }, name { n }, type { t }, bimode { dsp::BIDIR_NONE }
        {
            get_sampling_rate = ((t == dsp::TYPE_BIDIR) ||
                                    (t == dsp::TYPE_RESAMPLER) ||
                                    (t == dsp::TYPE_SOURCE))
                                ? true
                                : false;
        }

        const interface iface;
        void *block;
        const char *name;
        bool  get_sampling_rate;
        const dsp::block_type type;
        dsp::bidir_mode bimode;
    };

    std::vector<link> m_Chain;

    util::aligned_ptr<float>                m_fBuff[2];
    util::aligned_ptr<rm_math::complex_f>   m_cBuff[2];

    // Holders for owned pointers to block instances - they get released by the clear() method or
    // by instance destruction.
    std::vector<std::unique_ptr<dsp::block<dsp::func_ff>>> m_FloatBlocks;
    std::vector<std::unique_ptr<dsp::block<dsp::func_fc>>> m_FloatCmplxBlocks;
    std::vector<std::unique_ptr<dsp::block<dsp::func_cf>>> m_CmplxFloatBlocks;
    std::vector<std::unique_ptr<dsp::block<dsp::func_cc>>> m_CmplxBlocks;

    static constexpr uint8_t IN_MASK        = 1;
    static constexpr uint8_t OUT_MASK       = 2;

    //! \endcond

    bool isValidLink(uint8_t link1, uint8_t link2)
    {
        // Check if the output type of link1 is the same as the input type of link2
        return (((link1 & OUT_MASK) >> 1) == (link2 & IN_MASK));
    }

    // Handles the processing of each link during an iteration.
    template<typename T, typename U, typename V>
    void handleLink(const link &lnk, dsp::rate_t &rate, util::aligned_ptr<U> &in, util::aligned_ptr<V> &out)
    {
        auto blk = static_cast<dsp::block<T> *>(lnk.block);

        // Set the sampling rate of the current link to what was set by a previous link.
        blk->setSamplingRate(rate);

        // Update the bidirectional mode (N/A if this isn't a bidirectional endpoint but it doesn't hurt)
        blk->setBidirMode(lnk.bimode);

        m_LinkTrace.print(ID, "Process link %s: wr sr %u\n", lnk.name, rate);

        // Call the processor.
        blk->getProcesser()(in, out);

        // If the current link is a source or resampler, then
        // copy the sampling rate from the current link for the next links.
        if (lnk.get_sampling_rate)
        {
            rate = blk->getSamplingRate();
            m_LinkTrace.print(ID, "rd sr %u\n", rate);
        }
    }

    //! \cond

    const char *m_Name;
    bool m_IsChecked;
    uint8_t m_FIdx;
    uint8_t m_CIdx;

    static constexpr char const *ID = "CHAIN";

    util::trace<> m_Trace;
    util::trace<> m_LinkTrace;

    //! \endcond
};

}
