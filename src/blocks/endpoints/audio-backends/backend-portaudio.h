// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <portaudio.h>

#ifdef __linux__
#include <pa_linux_alsa.h>
#endif

// This is a "private" class which should only be accessed via the audio interface (audio.h).
// Endpoints generally use a singleton object since they interface with a global resource.
// Rather than using the traditional virtual or dispatch wrapper methods, we're using the CRTP
// design pattern which avoids a virtual function table and wrapper overhead.

class pa_impl : public audio<pa_impl>
{
public:

    pa_impl();
    ~pa_impl();

    void get_devices_impl(std::vector<device_info> &info);
    void print_info_impl();
    int set_device_impl(const std::array<int,2> devs, const rate_t rate,
                        const uint32_t blkSz,
                        const uint32_t bufSz,
                        const void *data = nullptr);

    int start_stream_impl();
    void process_samples_impl(const util::aligned_ptr<float> &inBlock, util::aligned_ptr<float> &outBlock);

private:
    std::vector<device_info> m_DevInfo;

    std::unique_ptr<std::thread> m_WriteTh;
    std::unique_ptr<std::thread> m_ReadTh;

    std::unique_ptr<util::ring_buffer<float>> m_WriteBuff;
    std::unique_ptr<util::ring_buffer<float>> m_ReadBuff;

    void writeThread(util::ring_buffer<float> &wBuff);
    void readThread(util::ring_buffer<float> &rBuff);

    void setupThread(uint32_t size, bool in, bool out);

    PaStream *m_Stream;
    bool m_ThreadsActive;
    std::mutex m_IOMtx;
    uint32_t m_BuffSz;

    PaStreamParameters m_InputParams;
    PaStreamParameters m_OutputParams;

    int set_linux_device(const std::array<int,2> devs, const rate_t rate,
                            const uint32_t blkSz,
                            const uint32_t bufSz,
                            const void *data = nullptr);

    int set_mac_device(const std::array<int,2> devs, const rate_t rate,
                        const uint32_t blkSz, const uint32_t bufSz);

    int set_windows_device(const std::array<int,2> devs, const rate_t rate,
                            const uint32_t blkSz, const uint32_t bufSz);
};
