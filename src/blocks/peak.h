#pragma once

#include <cmath>
#include "block.h"

namespace util {

/*! \brief Find peaks in a sample stream.
 *
 * Given three samples, this will determine if the middle sample is a peak or not.
 * This is designed to handle streams of samples so to find the peaks, send each
 * sample into the *get* function and check the boolean return code to see if
 * a peak was found.
 *
 * \note Since this requires three samples, a peak is reported on the sample *following*
 * the actual sample that is the peak.
 *
 */

template<typename T>
class peak : public dsp::block
{
public:
    peak() : m_Cnt(0), 
             m_Syms{ {0, 0}, {0, 0}, {0, 0} } {}

    //! Send a sample in and check if a peak was detected.
    //! @param [in]  sym    The sample to consider with the previous two samples.
    //! @param [out] pk     The sample that is the peak, if one was detected.
    //! @return If a peak was detected or not.
    bool get(const T sym, T &pk)
    {
        bool ret = false;

        m_Syms[2] = m_Syms[1];
        m_Syms[1] = m_Syms[0];

        m_Syms[0].val = sym;
        m_Syms[0].absVal = std::abs(sym);

        ++m_Cnt;

        if (m_Cnt == 3)
        {
            if ((m_Syms[0].absVal < m_Syms[1].absVal) && (m_Syms[2].absVal < m_Syms[1].absVal))
            {
                pk = m_Syms[1].val;
                ret = true;
            }

            m_Cnt = 1;
        }

        return ret;
    }

private:
    struct sym_t
    {
        T val;
        T absVal;
    };

    uint8_t m_Cnt;
    sym_t m_Syms[3];
};
}
