// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <memory>
#include "audio.h"

using namespace dsp::endpoints;

#if defined(INCLUDE_AUDIO_ENDPOINTS) && defined(AUDIO_BACKEND_PORTAUDIO)

static bool instantiated = false;

#ifdef AUDIO_ENDPOINT_DYNAMIC
std::unique_ptr<audio_ep> dsp::endpoints::make_audio_endpoint()
{
    assert(!instantiated);

    std::unique_ptr<audio_ep> res;

    if (!instantiated)
    {
        // The one and only instance
        std::unique_ptr<pa_impl> theInstance;

        theInstance = std::make_unique<pa_impl>();

        res = std::move(theInstance);

        instantiated = true;
    }

    // If asserts are compiled out and 'instantiated' is true, then the result is an
    // empty 'unique_ptr' going back to the caller.
    return res;
}
#else
static pa_impl theInstance;

audio_ep *dsp::endpoints::make_audio_endpoint()
{
    instantiated = true;
    return &theInstance;
}

#endif // AUDIO_ENDPOINT_STATIC

pa_impl::pa_impl() : m_Stream { nullptr }, m_ThreadsActive { false }, m_BuffSz { 0 }
{
#ifndef NDEBUG
    auto err = Pa_Initialize();

    assert(err == paNoError);
#else
    Pa_Initialize();
#endif

    m_Trace.print(ID, "Using backend %s\n", Pa_GetVersionText());
}

pa_impl::~pa_impl()
{
    m_ThreadsActive = false;

    if (m_ReadTh.get())
        m_ReadTh->join();

    if (m_WriteTh.get())
        m_WriteTh->join();

    instantiated = false;

    Pa_CloseStream(m_Stream);
    Pa_Terminate();
}

void pa_impl::print_info_impl()
{
    auto tr = std::make_unique<util::trace<true>>();

    auto cnt = Pa_GetDeviceCount();

    tr->print(ID, "Devices:\n");
    for (int i=0;i < cnt;i++)
    {
        auto info = Pa_GetDeviceInfo(i);

        tr->print(ID, "%d: %s, input chans %d, output chans %d, def sample rate %u\n",
            i, info->name, info->maxInputChannels, info->maxOutputChannels,
            static_cast<rate_t>(info->defaultSampleRate));
    }

    cnt = Pa_GetHostApiCount();

    tr->print(ID, "Host APIs:\n");
    for (int i=0;i < cnt;i++)
    {
        auto info = Pa_GetHostApiInfo(i);

        tr->print(ID,"%d: %s, device count %d, id %u\n",
            i, info->name, info->deviceCount, info->type);
    }
}

void pa_impl::get_devices_impl(std::vector<device_info> &info)
{
    auto cnt = Pa_GetDeviceCount();
    device_info dinfo;

    for (int i=0;i < cnt;i++)
    {
        auto inf = Pa_GetDeviceInfo(i);

        dinfo.name = inf->name;
        dinfo.numInputChannels = inf->maxInputChannels;
        dinfo.numOutputChannels = inf->maxOutputChannels;
        dinfo.index = i;
        dinfo.defaultSamplingRate = static_cast<rate_t>(inf->defaultSampleRate);

        info.push_back(dinfo);
    }
}

int pa_impl::set_device_impl(const std::array<int,2> devs, const rate_t rate,
                            const uint32_t blkSz,
                            const uint32_t bufSz,
                            const void *data )
{
#ifdef __linux__
    return static_cast<int>(set_linux_device(devs, rate, blkSz, bufSz, data));
#else
    assert(1);
#endif
}

int pa_impl::start_stream_impl()
{
    assert(m_Stream);

    auto res = Pa_StartStream(m_Stream);

    if (res == paNoError)
    {
        m_ThreadsActive = true;
        setupThread(m_BuffSz,
                    (m_InputParams.device != NO_DEVICE) ? true : false,
                    (m_OutputParams.device != NO_DEVICE) ? true : false);
    }

    return static_cast<int>(res);
}

void pa_impl::process_samples_impl(const util::aligned_ptr<float> &inBlock, util::aligned_ptr<float> &outBlock)
{
    if (m_BidirMode == BIDIR_SOURCE)
    {
        util::init_aligned_ptr_on_resize<float>(outBlock, m_BlockSize);
        m_ReadBuff->read(&outBlock[0], outBlock.size());
    }
    else
    {
        m_WriteBuff->write(inBlock.data(), inBlock.size());
    }
}

void pa_impl::writeThread(util::ring_buffer<float> &wBuff)
{
    auto buff = std::make_unique<float[]>(m_BlockSize);

    while(m_ThreadsActive)
    {
        wBuff.read(&buff[0], m_BlockSize);

        m_IOMtx.lock();
        auto res = Pa_WriteStream(m_Stream, static_cast<const void *>(buff.get()), m_BlockSize);
        m_IOMtx.unlock();

        std::lock_guard<std::mutex> lck(m_Mtx);
        if (res == paOutputUnderflowed)
            ++m_Stats.underflow;

        ++m_Stats.outputBlockCount;
    }
}

void pa_impl::readThread(util::ring_buffer<float> &rBuff)
{
    auto buff = std::make_unique<float[]>(m_BlockSize);

    while(m_ThreadsActive)
    {
        m_IOMtx.lock();
        auto res = Pa_ReadStream(m_Stream, static_cast<void *>(&buff[0]), m_BlockSize);
        m_IOMtx.unlock();

        rBuff.write(buff.get(), m_BlockSize);

        std::lock_guard<std::mutex> lck(m_Mtx);
        if (res == paInputOverflowed)
            ++m_Stats.overflow;

        ++m_Stats.inputBlockCount;
    }
}

void pa_impl::setupThread(uint32_t size, bool in, bool out)
{
    m_Trace.print(ID, "ring buff size: %lu\n", size);

    if (in)
    {
        m_ReadBuff = std::make_unique<util::ring_buffer<float>>(size);
        m_ReadTh = std::make_unique<std::thread>(&pa_impl::readThread, this, std::ref(*m_ReadBuff));
    }

    if (out)
    {
        m_WriteBuff = std::make_unique<util::ring_buffer<float>>(size);
        m_WriteTh = std::make_unique<std::thread>(&pa_impl::writeThread, this, std::ref(*m_WriteBuff));
    }
}

int pa_impl::set_linux_device(const std::array<int,2> devs, const rate_t rate,
                                const uint32_t blkSz,
                                const uint32_t bufSz,
                                const void *data)
{
    auto idx = devs;

    if (!bufSz || (bufSz & (bufSz - 1)))
    {
        m_Trace.print(ID, "Buffer size is not mod 2\n");
        return -1 ;
    }

    PaAlsaStreamInfo info[2]
    {
        { .size = sizeof(PaAlsaStreamInfo) },
        { .size = sizeof(PaAlsaStreamInfo) }
    };

    const alsa_params *params = nullptr;

    // On Linux, verify ALSA is available
    auto err = Pa_HostApiTypeIdToHostApiIndex(paALSA);

    if (err < 0)
    {
        m_Trace.print(ID, "ALSA not found!\n");
        return static_cast<int>(err);
    }

    if (data)
    {
        params = static_cast<const alsa_params *>(data);

        if (params->input)
        {
            PaAlsa_InitializeStreamInfo(&info[0]);
            idx[0] = paUseHostApiSpecificDeviceSpecification;
            info[0].deviceString = params->input;
        }

        if (params->output)
        {
            PaAlsa_InitializeStreamInfo(&info[1]);
            idx[1] = paUseHostApiSpecificDeviceSpecification;
            info[1].deviceString = params->output;
        }

        if (params->periodSize)
            PaAlsa_SetNumPeriods(params->periodSize);
    }

    // Good to setup the stream now.
    if ((idx[0] == paUseHostApiSpecificDeviceSpecification) || (idx[0] >= 0))
    {
        if (data) { assert(params->input); }

        m_InputParams.channelCount = 1;
        m_InputParams.device = idx[0];
        m_InputParams.sampleFormat = paFloat32;
        m_InputParams.suggestedLatency = static_cast<PaTime>(1.0f / rate * blkSz * 2);
        m_InputParams.hostApiSpecificStreamInfo = nullptr;

        if (idx[0] == paUseHostApiSpecificDeviceSpecification)
            m_InputParams.hostApiSpecificStreamInfo = static_cast<void *>(&info[0]);
    }
    else
        m_InputParams.device = NO_DEVICE;

    if ((idx[1] == paUseHostApiSpecificDeviceSpecification) || (idx[1] >= 0))
    {
        if (data) { assert(params->output); }

        m_OutputParams.channelCount = 1;
        m_OutputParams.device = idx[1];
        m_OutputParams.sampleFormat = paFloat32;
        m_OutputParams.suggestedLatency = static_cast<PaTime>(1.0f / rate * blkSz * 2);
        m_OutputParams.hostApiSpecificStreamInfo = nullptr;

        if (idx[1] == paUseHostApiSpecificDeviceSpecification)
            m_OutputParams.hostApiSpecificStreamInfo = static_cast<void *>(&info[1]);
    }
    else
        m_OutputParams.device = NO_DEVICE;

    err = Pa_OpenStream(&m_Stream,
                        (m_InputParams.device != NO_DEVICE) ? (static_cast<const PaStreamParameters *>(&m_InputParams)) : nullptr,
                        (m_OutputParams.device != NO_DEVICE) ? (static_cast<const PaStreamParameters *>(&m_OutputParams)) : nullptr,
                        rate, blkSz, paNoFlag, nullptr, nullptr);

    if (err == paNoError)
    {
        m_BlockSize = blkSz;
        m_BuffSz = bufSz;

        // Needed if we decide to switch to the callback method. Currently we're using the
        // synchronous method with read and write threads.
        // PaAlsa_EnableRealtimeScheduling(&m_Stream, 1);
    }

    return static_cast<int>(err);
}

int pa_impl::set_mac_device(const std::array<int,2> devs, const rate_t rate,
                            const uint32_t blkSz, const uint32_t bufFz)
{
    return static_cast<int>(paHostApiNotFound);
}

int pa_impl::set_windows_device(const std::array<int,2> devs, const rate_t rate,
                            const uint32_t blkSz, const uint32_t bufSz)
{
    return static_cast<int>(paHostApiNotFound);
}

#endif  // defined(INCLUDE_AUDIO_ENDPOINTS) && defined(AUDIO_BACKEND_PORTAUDIO)
