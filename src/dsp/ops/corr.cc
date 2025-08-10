// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

//! \file corr.cc

#include <volk/volk.h>
#include <cstring>
#include <corr.h>
#include <cassert>

#include "cmove.h"
#include "cmemset.h"

using namespace dsp;

corr::corr(const size_t n, const buffer_t type) :
    m_Type(type),
    m_Size(n),
    m_CountX(0),
    m_CountY(0),
    m_fX(nullptr),
    m_fY(nullptr),
    m_cX(nullptr),
    m_cY(nullptr)
{
    if (type == buffer_type_real)
    {
        m_fX = (float *)volk_malloc(n * sizeof(float), volk_get_alignment());
        m_fY = (float *)volk_malloc(n * sizeof(float), volk_get_alignment());
    }
    else
    {
        m_cX = (std::complex<float> *)volk_malloc(n * sizeof(std::complex<float>), volk_get_alignment());
        m_cY = (std::complex<float> *)volk_malloc(n * sizeof(std::complex<float>), volk_get_alignment());
    }

    resetX();
    resetY();
}

corr::~corr()
{
    if (m_fX) volk_free(m_fX);
    if (m_fY) volk_free(m_fY);
    if (m_cX) volk_free(m_cX);
    if (m_cY) volk_free(m_cY);
}

void corr::setX(const float *x)
{
    assert(m_fX != nullptr);

    std::memcpy(m_fX, x, m_Size * sizeof(float));
    m_CountX = 0;
}

void corr::setX(const std::complex<float> *x)
{
    assert(m_cX != nullptr);

    for (size_t i=0;i < m_Size;i++)
    {
        m_cX[i].real(x[i].real());
        m_cX[i].imag(x[i].imag());
    }

    m_CountX = 0;
}

void corr::setY(const float *y)
{
    assert(m_fY != nullptr);

    std::memcpy(m_fY, y, m_Size * sizeof(float));
    m_CountY = 0;
}

void corr::setY(const std::complex<float> *y)
{
    assert(m_cY != nullptr);

    util::cmove<float>(m_cY, y, m_Size);
    m_CountY = 0;
}

void corr::calculate(float &coeff)
{
    volk_32f_x2_dot_prod_32f(&coeff, m_fX, m_fY, m_Size);
}

void corr::calculate(std::complex<float> &coeff)
{
    volk_32fc_x2_dot_prod_32fc(&coeff, m_cX, m_cY, m_Size);
}

float corr::addX(const float x)
{
    assert(m_fX != nullptr);
    return add(x, m_CountX, m_fX);
}

std::complex<float> corr::addX(const std::complex<float> x)
{
    assert(m_cX != nullptr);
    return add(x, m_CountX, m_cX);
}

float corr::addY(const float y)
{
    assert(m_fY != nullptr);
    return add(y, m_CountY, m_fY);
}

std::complex<float> corr::addY(const std::complex<float> y)
{
    assert(m_cY != nullptr);
    return add(y, m_CountY, m_cY);
}

float corr::add(const float val, size_t &cnt, float *buff)
{
    float res = 0;

    if (cnt == m_Size)
    {
        std::memmove(buff, &buff[1], (m_Size - 1) * sizeof(float));
        buff[m_Size - 1] = val;
    }
    else
        buff[cnt++] = val;

    calculate(res);

    return res;
}

std::complex<float> corr::add(const std::complex<float> val, size_t &cnt, std::complex<float> *buff)
{
    std::complex<float> res;

    if (cnt == m_Size)
    {
        util::cmove(buff, &buff[1], m_Size - 1);

        buff[m_Size - 1].real(val.real());
        buff[m_Size - 1].imag(val.imag());
    }
    else
    {
        buff[cnt].real(val.real());
        buff[cnt++].imag(val.imag());
    }

    calculate(res);

    return res;
}

void corr::resetX()
{
    zeroX();
    m_CountX = 0;
}

void corr::resetY()
{
    zeroY();
    m_CountY = 0;
}

void corr::zeroX()
{
    assert((m_fX != nullptr) || (m_cX != nullptr));

    if (m_Type == buffer_type_real)
        std::memset(m_fX, 0, m_Size * sizeof(float));
    else
        util::cmemset(m_cX, std::complex<float>{0, 0}, m_Size);
}

void corr::zeroY()
{
    assert((m_fY != nullptr) || (m_cY != nullptr));

    if (m_Type == buffer_type_real)
        std::memset(m_fY, 0, m_Size * sizeof(float));
    else
        util::cmemset<float>(m_cY, std::complex<float>{0, 0}, m_Size);
}
