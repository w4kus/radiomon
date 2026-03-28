// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <vector>
#include <array>

#include "poly-subfilter.h"

namespace util {

/*! \file polyphase.h
 *
 * \brief Helper to Build Polyphase FIR Structures
 *
 * @param [in] taps  A 2D *std::vector* containing the decomposed FIR coefficients to use.
 *
 * @return A *std::vector* containing instances of type *comps::poly_subfilter*
 *         with the passed in coefficients.
 */
template<typename T>
std::vector<comps::poly_subfilter<T>> polyBuildFilter(const std::vector<std::vector<T>> &taps, uint16_t gain)
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);

    std::vector<comps::poly_subfilter<T>> ret;

    if (gain > 1)
    {
        // A vector of vectors declaration needs to call an explicit contructor (i.e., don't use {} initialization)
        std::vector<std::vector<T>> temp(taps.size(), std::vector<T>(taps[0].size()));

        for (size_t i=0;i < taps.size();i++)
        {
            for (size_t j=0;j < taps[i].size();j++)
                temp[i][j] = taps[i][j] * gain;
        }

        for (size_t i=0;i < taps.size();i++)
            ret.push_back(comps::poly_subfilter<T>(temp[i]));
    }
    else
    {
        for (size_t i=0;i < taps.size();i++)
            ret.push_back(comps::poly_subfilter<T>(taps[i]));
    }

    return ret;
}

/*!
 * \brief Helper to Build Polyphase FIR Structures
 *
 * @param [in] taps  A 2D *std::array* containing the decomposed FIR coefficients to use.
 *                   *R* is the number of rows (sub-filters), *C* is the number of columns
 *                   (size of each sub-filter).
 *
 * @return A *std::vector* containing instances of type *comps::poly_subfilter*
 *         with the passed in coefficients.
 */
template<typename T, size_t R, size_t C>
std::vector<comps::poly_subfilter<T>> polyBuildFilter(const std::array<std::array<T, C>, R> &taps, uint16_t gain)
{
    static_assert((std::is_floating_point<T>::value == std::true_type()) || util::is_std_complex_v<T>);

    std::vector<comps::poly_subfilter<T>> ret;

    if (gain > 1)
    {
        // A vector of vectors declaration needs to call an explicit contructor (i.e., don't use {} initialization)
        std::vector<std::vector<T>> temp(R, std::vector<T>(C));

        for (size_t i=0;i < taps.size();i++)
        {
            for (size_t j=0;j < taps[i].size();j++)
                temp[i][j] = taps[i][j] * gain;
        }

        for (size_t i=0;i < taps.size();i++)
            ret.push_back(comps::poly_subfilter<T>(temp[i]));
    }
    else
    {
        for (size_t i=0;i < taps.size();i++)
            ret.push_back(comps::poly_subfilter<T>(taps[i]));
    }

    return ret;
}
}
