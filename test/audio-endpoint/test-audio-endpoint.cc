// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include "audio.h"
#include "chain.h"
#include "signal-source.h"
#include "menu.h"

constexpr int SAMPLE_RATE = 48000;
constexpr int SAMPLE_SIZE = 480;    // 100ms blocks - FIXME high latency
constexpr int BUFFER_SIZE = 1024;   // mod-2 size for util::ring_buffer

using audioif = dsp::endpoints::audio_ep;

static bool running;

static bool menu_gain(const std::string &input);
static bool menu_quit(const std::string &input);
static bool menu_stats(const std::string &input);
static bool menu_freq(const std::string &input);

// Create the audio sink - this is a pointer to either a statically or dynamically allocated
// instance but either way, hardware is a globally shared resource so only one call is needed.
static auto *aep = dsp::endpoints::make_audio_endpoint();

// Create a signal source block to be added to the chain as a reference so we can adjust some parameters
// like frequency and gain during runtime. The hardware endpoint will act as the master clock / throttle.
static dsp::endpoints::signal_source_ff sig { SAMPLE_SIZE, 800, SAMPLE_RATE, 0.0f, 0.1f };

int main(int argc, char **argvp)
{
    // Print stuff from the audio endpoint.
    aep->print_info();

    std::vector<audioif::device_info> devices;
    aep->get_devices(devices);

    // Create a test menu
    util::menu menu;
    util::menu::entry entry;

    menu.set_help(true);

    entry.key = 'G';
    entry.description = "Set the signal gain (0.0 - 1.0)";
    entry.prompt = "gain";
    entry.handler = menu_gain;
    menu.add(entry);

    entry.key = 'F';
    entry.description = "Set the signal frequency (Hz)";
    entry.prompt = "freq";
    entry.handler = menu_freq;
    menu.add(entry);

    entry.key = 'P';
    entry.description = "Print audio status";
    entry.prompt = nullptr;
    entry.handler = menu_stats;
    menu.add(entry);

    entry.key = 'Q';
    entry.description = "Exit";
    entry.prompt = nullptr;
    entry.handler = menu_quit;
    menu.add(entry);

    // Create a chain
    util::chain chain;

    // Set up the audio endpoint
#if 1
    // ALSA specific
    #if 0

    // Bidirectional example (mic -> speaker)
    // NOTE that if this test app is run as a "nice" process with the default nice value in Linux
    // and at a 48K sampling rate, overruns/underruns will probably occur.  Since radio apps are
    // half duplex, this shouldn't normally be an issue. But if full duplex is desired, then
    // process priority and scheduling should be considered. Your mileage will vary with other systems.
    // Also consider reducing the sampling rate.
    audioif::alsa_params params =
    {
        .input = "plughw:CARD=PCH,DEV=0",
        .output = "plughw:CARD=PCH,DEV=0",
        .periodSize = 0 // set to zero to use the library default.
    };

    auto res = aep->set_device({0, 0}, SAMPLE_RATE, SAMPLE_SIZE, BUFFER_SIZE,
                                static_cast<const void *>(&params));

    // Audio source
    chain.add(*aep, "AUDIO_SRC");

    #else

    // Sink only example (signal source -> speaker)
    audioif::alsa_params params =
    {
        .input =  nullptr,
        .output = "plughw:CARD=PCH,DEV=0",
        .periodSize = 0 // set to zero to use the library default.
    };

    auto res = aep->set_device({audioif::NO_DEVICE, 0}, SAMPLE_RATE, SAMPLE_SIZE, BUFFER_SIZE,
                                static_cast<const void *>(&params));


    // Signal source
    chain.add(sig, "SIGNAL_SRC");

    #endif
#else
    // Generic API - use the device index or NO_DEVICE. You'll need to determine the proper index
    // for the device. The "print_info()" or "get_devices()" audio endpoint methods can be used for this.

    // Sink only example (signal source -> speaker)
    auto res = aep->set_device({audioif::NO_DEVICE, 2}, SAMPLE_RATE, SAMPLE_SIZE, BUFFER_SIZE);

    // Signal source
    chain.add(sig, "SIGNAL_SRC");
#endif

    if (res)
    {
        printf("Error setting up the device: %d\n", res);
        return -1;
    }

    // audio sink
    chain.add(*aep, "AUDIO_SINK");

    if (!chain.setup())
    {
        printf("Chain setup failed\n");
        return -1;
    }

    running = true;
    menu.display();

    // start the audio sink
    res = aep->start_stream();

    if (!res)
    {
        // main loop
        while(running)
        {
            chain.iterate();

            // handle user input
            menu.processInput();
        }
    }
    else
    {
        printf("Stream start failed [%d]\n", res);
        return -1;
    }

    return 0;
}

// Menu item handlers
bool menu_gain(const std::string &input)
{
    auto val = std::stof(input);

    if (val >= 0.0 && val <= 1.0)
        sig.setGain(val);

    return true;
}

bool menu_freq(const std::string &input)
{
    auto val = std::stoul(input);
    sig.setFrequency(val);

    return true;
}

bool menu_quit(const std::string &input)
{
    printf("Exiting\n");
    running = false;
    return true;
}

bool menu_stats(const std::string &input)
{
    audioif::stream_stats stats;
    aep->get_stats(stats);

    printf("\nof=%u uf=%u in=%u out=%u\n",
            stats.overflow,
            stats.underflow,
            stats.inputBlockCount,
            stats.outputBlockCount);

    return true;
}
