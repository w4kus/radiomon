// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdlib.h>
#include <stdio.h>

#include "text-file-sink.h"
#include "signal-source.h"
#include "chain.h"

constexpr dsp::rate_t   Fs      = 48000;
constexpr dsp::rate_t   F       = 800;
constexpr size_t        blkSz   = Fs/F;

int main(int argc, char **argvp)
{
    util::chain chain("FILE_SINK_TEST");

    chain.add(std::make_unique<dsp::endpoints::signal_source_ff>(blkSz, F, Fs), "SIG_SOURCE");
    chain.add(std::make_unique<dsp::endpoints::text_file_sink_ff>("test-text-file-sink-ff.txt"), "TXT_FILE_SINK");

    if (!chain.setup())
    {
        printf("chain check failed\n");
        return -1;
    }

    chain.iterate();
    chain.clear();

    chain.add(std::make_unique<dsp::endpoints::signal_source_cc>(blkSz, F, Fs), "SIG_SOURCE");
    chain.add(std::make_unique<dsp::endpoints::text_file_sink_cc>("test-text-file-sink-cc.txt"), "TXT_FILE_SINK");

    if (!chain.setup())
    {
        printf("chain check failed\n");
        return -1;
    }

    chain.iterate();

    return 0;
}
