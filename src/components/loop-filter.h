// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include "delay.h"

namespace comps {

/*! \brief Proportional / Integrator Loop Filter
 *
 * This is a PI filter for use in feedback loops such as *PLLs* and *FLLs*.
 * 
 * Although it's simple, the choice of **Kp** and **Ki** is critical to its operation.
 * In general, the goal of a loop filter is to remove noise from error detectors
 * (e.g., a phase error detector of a PLL), and control the dynamic nature of the loop.
 * 
 * Following *GNU Radio*'s (GR) lead and with special thanks to Qasim Chaudhari's wonderful
 * e-book [Wireless Communications from the Ground Up - An SDR Perspective](https://wirelesspi.com/)
 * for decoding GR's intentions, one can simply choose the loop bandwidth to get a usable approximation
 * of the coefficients.
 * 
 * A good rule-of-them is to choose a bandwidth of 1% of the total bandwidth which, when
 * expressed in radians, would be **bw = 2*pi/100**. Chaudhari's findings dictate the following:
 * 
 * * Kp = 4*bw
 * * Ki = Kp^2/4
 * 
 * In general, again using GR as a guide, you can vary this between **pi/100** and **2*pi/100**
 * and get reasonable results. There are many resources on the web and in communication texts 
 * about calculating PI coefficents but the above simple calculations should work in most situations.
*/

class loopfilt
{

public:

    loopfilt() = delete;

    //! Create an instance
    //! @param [in] Kp  The proportional constant.
    //! @param [in] Ki  The integral constant.
    loopfilt(float Kp, float Ki) : m_Kp { Kp }, m_Ki { Ki }
    { }

    //! Send a value through the filter.
    //! @return The filtered result.
    float filter(float value);

private:
    float m_Kp;
    float m_Ki;

    delay<float> m_Delay;
};

}
