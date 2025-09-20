// Copyright (c) 2025 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <complex>

namespace util {

/*! \brief Calculate correlation coefficents between two signals.
 *
 * This will find the correlation between two signals of length *n*.
 * Mathematically, correlation is identical to convolution with the only
 * difference being that, by definition, one of the signals must be time reversed
 * with convolution. Thus, you could use \ref dsp::conv or \ref dsp::fconv for correlation as well.
 * Typically when correlating you work on one or two symbols/samples at a time, e.g.,
 * when searching for a known sequence using data aided synchronization
 * methods, so this class provides a friendly interface for that, including
 * buffer management and the use of the [Volk](https://www.libvolk.org/) library for speed.
 *
 * Instantiating this class will create two buffers of length *n*, dubbed
 * *X* and *Y*. Use the methods below to manipulate and calculate
 * coefficents when needed. You can correlate either **float** or **complex** types
 * but you cannot mix them.
 *
 * \warning All public methods use *assert* to ensure that the passed in values are of the
 * same type as the instantiated type.
 *
 * \note In general, the amount of data in the *X* and *Y* buffers are usually the same
 * by completely filling the buffers before correlation but it's not necessary or enforced.
 * Do keep in mind that this class will always calculate the coefficient over the entire buffer
 * regardless of the amount of valid data in each.
 *
 */

class corr
{
public:

    corr() = delete;
    corr(const corr &) = delete;
    corr& operator=(const corr&) = delete;

    ~corr();

    typedef enum
    {
        buffer_type_real,   /**< float */
        buffer_type_complex /**< std::complex<float> */
    }buffer_t;

    //! Create an instance for signals of a certain length.
    //! @param [in] n       The number samples in the signal
    //! @param [in] type    the type of data
    corr(const size_t n, const buffer_t type);

    //! Set buffer *X* to a sequence of samples of type *float*.
    //! If there's is not enough space, the sequence is truncated
    //! to fit. The buffer is cleared before copying the samples.
    //! *X* is unchanged if the buffer type isn't of type *float*.
    //! @param [in] x Pointer to the samples
    void setX(const float *x);

    //! Set buffer *X* to a sequence of samples of type *std::complex<float>*.
    //! If there's is not enough space, the sequence is truncated
    //! to fit. The buffer is cleared before copying the samples.
    //! *X* is unchanged if the buffer type isn't of type *std::complex<float>*.
    //! @param [in] x Pointer to the samples
    void setX(const std::complex<float> *x);

    //! Set buffer *Y* to a sequence of samples of type *float*.
    //! If there's is not enough space, the sequence is truncated
    //! to fit. The buffer is cleared before copying the samples.
    //! *X* is unchanged if the buffer type isn't of type *float*.
    //! @param [in] y Pointer to the samples
    void setY(const float *y);

    //! Set buffer *Y* to a sequence of samples of type *std::complex<float>*.
    //! If there's is not enough space, the sequence is truncated
    //! to fit. The buffer is cleared before copying the samples.
    //! *X* is unchanged if the buffer type isn't of type *std::complex<float>*.
    //! @param [in] y Pointer to the samples
    void setY(const std::complex<float> *y);

    //! Calculate the correlation coefficent of *X* and *Y*.
    //! @param [out] coeff The coefficient of the current content.
    void calculate(float &coeff);

    //! Calculate the correlation coefficent of *X* and *Y*.
    //! @param [out] coeff The coefficient of the current content.
    void calculate(std::complex<float> &coeff);

    //! Add a sample of type *float* to buffer *X*. If the buffer is full,
    //! it is shifted left removing the oldest sample.
    //! *X* is unchanged if the buffer type isn't of type *float*.
    //! @param [in] x The new sample to add
    //! @return The correlation coefficient after adding the sample.
    float addX(const float x);

    //! Add a sample of type *std::complex<float>* to buffer *X*.
    //! If the buffer is full, it is shifted left removing the oldest sample.
    //! *X* is unchanged if the buffer type isn't of type *std::complex<float>*.
    //! @param [in] x The new sample to add
    //! @return The correlation coefficient after adding the sample.
    std::complex<float> addX(const std::complex<float> x);

    //! Add a sample of type *float* to buffer *Y*. If the buffer is full,
    //! it is shifted left removing the oldest sample.
    //! *Y* is unchanged if the buffer type isn't of type *float*.
    //! @param [in] y The new sample to add
    //! @return The correlation coefficient after adding the sample.
    float addY(const float x);

    //! Add a sample of type *std::complex<float>* to buffer *Y*.
    //! If the buffer is full, it is shifted left removing the oldest sample.
    //! *Y* is unchanged if the buffer type isn't of type *std::complex<float>*.
    //! @param [in] y The new sample to add
    //! @return The correlation coefficient after adding the sample.
    std::complex<float> addY(const std::complex<float> y);

    //! Zero out the *X* buffer and reset the buffer counter.
    //! @param none
    void resetX();

    //! Zero out the *Y* buffer and reset the buffer counter.
    //! @param none
    void resetY();

    //! Zero out the *X* buffer but preserve the buffer count.
    //! @param none
    void zeroX();

    //! Zero out the *Y* buffer but preserve the buffer count.
    //! @param none
    void zeroY();

private:

    buffer_t m_Type;
    size_t m_Size;
    size_t m_CountX;
    size_t m_CountY;

    float *m_fX;
    float *m_fY;

    std::complex<float> *m_cX;
    std::complex<float> *m_cY;

    float add(const float val, size_t &cnt, float *buff);
    std::complex<float> add(const std::complex<float> val, size_t &cnt, std::complex<float> *buff);
};
}
