// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <type_traits>
#include <functional>
#include <vector>
#include <memory>
#include <cassert>
#include <string>
#include <array>
#include <mutex>
#include <thread>

#include "block.h"
#include "trace.h"
#include "ring-buffer.h"
#include "timer.h"

namespace dsp { namespace endpoints {

/*! \brief Bidirectional Audio Endpoint Interface
 *
 * This block is the generic interface for sound hardware endpoints. This is primarily for
 * amateur radio since AFSK via the audio or mic port on a radio is widely used for digital
 * communication. Backends, located in the *audio-backends* directory, handle the
 * hardware and/or library specifics. For that reason, this block separates the definitions from
 * the implementation and you can use a compile switch to dictate which backend is used.
 * As of this writing, only PortAudio is supported. Rather than specifying the flag
 * globally in the top level meson.build file, each radio app must specify which backend
 * to use via the *cpp_flags* parameter in the *executable* function of the *meson.build* file.
 *
 * This is designed to offer only the necessary methods needed to source and sink a chain and to
 * cover most platforms from embedded enviroments, with or without an OS and little to no drivers,
 * to full fledged systems. As such some of the methods below might not need to be implemented.
 * Backend developers should at least provide an empty handler for unneeded methods.
 *
 * Currently supported backend compile switches:
 * - AUDIO_BACKEND_PORTAUDIO -- PortAudio
 *
 * ********************************************************************************************
 * \brief PortAudio Usage Notes
 *
 * PortAudio is essentially a common wrapper around several host audio APIs. As such, you can
 * choose which one to use and the input and output device need not be the same although only
 * one audio instance is needed. This module uses the following APIs for the three supported systems:
 *
 * * Linux   - ALSA. You can either specify the PA device index or the ALSA name string (e.g.,
 *             "plughw:CARD=CODEC,DEV=0") which is passed in the *data* parameter of *set_device*
 *             as a pointer of type *alsa_params*.
 *             The *bufSz* parameter of *set_device* is required and must be mod 2 in size.
 * * Mac OS  - Core Audio (not yet implemented)
 * * Windows - WMME (not yet implemented)
 *
 */

template<typename T>
class audio : public block<func_ff>
{

public:

#ifdef AUDIO_BACKEND_PORTAUDIO
    static constexpr int NO_DEVICE = -1;
#endif

    struct device_info
    {
        //! device index
        int index;

        //! device name
        std::string name;

        //! The number of input channels available
        int numInputChannels;

        //! The number of output channels available
        int numOutputChannels;

        //! The default sampling rate
        rate_t defaultSamplingRate;
    };

    struct stream_stats
    {
        // The number of overflows on the input detected.
        uint32_t overflow;

        //! The number of underflows on the output detected.
        uint32_t underflow;

        //! The number of sample blocks read.
        uint32_t inputBlockCount;

        //! The number of sample blocks written.
        uint32_t outputBlockCount;
    };

#ifdef __linux__
    struct alsa_params
    {
        //! Input device string or nullptr
        const char *input;

        //! Output device string or nullptr
        const char *output;

        //! The number of buffer fragments - defaults to 4.
        //! Smaller numbers decrease latency while causing more interrupts from the hardware
        //! to be generated, thus more CPU usage. Larger values decrease this CPU load
        //! while increasing latency. Larger numbers also buffer more data which can aid
        //! in preventing overruns or underruns. In ALSA speak, this parameter is referred
        //! to as the period size. Set to zero to use the library default (4).
        int periodSize;
    };
#endif

    //! Get a list of devices on the system.
    //! @param [out] info  An empty vector to which the information is written.
    void get_devices(std::vector<device_info> &info)
    {
        static_cast<T*>(this)->get_devices_impl(info);
    }

    //! Print intormation about the audio system. The information is backend dependent.
    void print_info()
    {
        static_cast<T*>(this)->print_info_impl();
    }

    //! Set the device and parameters to use
    //! @param [in] devs    The indices of the devices to use. Index zero is the input device
    //!                     and index one is the output device. An index < 0 specifies no device (input only or
    //!                     output only). Both indices can contain the same device if it is bidirectional.
    //! @param [in] rate    The sampling rate to use.
    //! @param [in] blkSz   The requested block size for both input and output.
    //! @param [in] bufSz   The size of the interface buffers for the hardware if the backend requires it.
    //! @param [in] data    Optional backend specififc data. See the main comment block above.
    //! @return The backend specific return code; typically zero should be returned on success.
    int set_device(const std::array<int,2> devs, const rate_t rate,
                    const uint32_t blkSz,
                    const uint32_t bufSz = 0,
                    const void *data = nullptr)
    {
        assert(rate && blkSz);
        m_SamplingRate = rate;
        return static_cast<T*>(this)->set_device_impl(devs, rate, blkSz, bufSz, data);
    }

    //! Start the audio sample stream. This can be called any time after
    //! *set_device()* is called but before the chain is activated. The result code should
    //! be checked to determine if chain activation should proceed.
    //! @return The backend specific return code; typically zero should be returned on success.
    int start_stream()
    {
        return static_cast<T*>(this)->start_stream_impl();
    }

    //! Stop the audio sample stream. Calling this when the stream is already stopped
    //! is undefined and backend dependent.  The result code should
    //! be checked to determine if chain activation should proceed.
    //! @return The backend specific return code; typically zero should be returned on success.
    int stop_stream()
    {
        return static_cast<T*>(this)->stop_stream_impl();
    }

    //! Get/send a block of samples.
    //! @param [in]     inBlock   The samples to send to the hardware
    //! @param [out]    outBlock  The latest samples from the hardware.
    void process_samples(const util::aligned_ptr<float> &inBlock, util::aligned_ptr<float> &outBlock)
    {
        // Endpoint sources must set the sampling rate on each block processing call. In sink
        // mode it doesn't matter.
        block::m_SamplingRate = m_SamplingRate;
        static_cast<T*>(this)->process_samples_impl(inBlock, outBlock);
    }

    //! Get the current stream statistics
    //! @param [out]  state stream_stats structure
    void get_stats(stream_stats &stats)
    {
        std::lock_guard<std::mutex> lck(m_Mtx);
        stats = m_Stats;
    }

protected:

    rate_t m_SamplingRate;
    size_t m_BlockSize;

    stream_stats m_Stats;

    std::mutex m_Mtx;

    audio() : block<dsp::func_ff> { TYPE_BIDIR },
                                    m_SamplingRate { 0 },
                                    m_BlockSize { 0 },
                                    m_Stats { 0, 0, 0, 0 }
    {
        process = std::bind(&audio::process_samples, this, std::placeholders::_1, std::placeholders::_2);
    }

    static constexpr char const *ID = "AUDIO_EP";
    util::trace<> m_Trace;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// To include audio endpoints in your app:
// * define INCLUDE_AUDIO_ENDPOINTS
// * define the audio backend (currently AUDIO_BACKEND_PORTAUDIO is the only one supported right now)
// * optionally define AUDIO_ENDPOINT_DYNAMIC for a dynamic instance; otherwise a static instance is created
//
// Global macro to include audio endpoints
#ifdef INCLUDE_AUDIO_ENDPOINTS

// Include the requested backend. All implementations must define the proper "audio_ep" type.
//
// PortAudio
#ifdef AUDIO_BACKEND_PORTAUDIO
#include "backend-portaudio.h"
using audio_ep = audio<pa_impl>;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// Common audio endpoint initializaton function
#ifndef AUDIO_ENDPOINT_DYNAMIC

// A static instance is instatiated at program startup. A call to make_audio_endpoint()
// returns a pointer to it and multiple calls will return the same instance.
audio_ep *make_audio_endpoint();

#else

// Transfers ownership of a unique_ptr containing the global instance of audio_ep to the caller.
// Only one call to make_audio_endpoint() is allowed and additional calls will either return an
// empty unique_ptr or gemerate an assert if compiled in.
std::unique_ptr<audio_ep> make_audio_endpoint();

#endif

#endif // INCLUDE_AUDIO_ENDPOINTS

}}
