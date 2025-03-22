#pragma once

namespace dmr {
    // 4FSK symbol  bits   deviation   Normalized
    //      +3      0b01   +1.944 Khz     1.00
    //      +1      0b00   +0.648 KHz     0.33
    //      -1      0b10   -0.648 KHz    -0.33
    //      -3      0b11   -1.944 KHz    -1.0


    // 4800 baud
    // 1ms = 4.8 sym per msec = 9.6 bpms
    static constexpr int        SAMPLES_PER_SYMBOL              = 5;    // 24KS/s

    // 5ms sync / embedded signal word
    static constexpr int        SYNC_WORD_SIZE_BITS             = 48;
    static constexpr int        SYNC_WORD_SIZE_BYTES            = SYNC_WORD_SIZE_BITS / 8;
    static constexpr int        SYNC_WORD_SYMBOL_NUM            = SYNC_WORD_SIZE_BITS / 2;

    // 2.5ms guard time (MS), CACH (BS)
    static constexpr int        TDMA_CACH_SIZE_BITS             = 24;
    static constexpr int        TDMA_CACH_SIZE_BYTES            = TDMA_CACH_SIZE_BITS / 8;
    static constexpr int        TDMA_CACH_SYMBOL_NUM            = TDMA_CACH_SIZE_BITS / 2;

    // Burst structure
    static constexpr int        BURST_FRAME_SIZE_BITS           = 264 + TDMA_CACH_SIZE_BITS;
    static constexpr int        BURST_FRAME_SIZE_BYTES          = BURST_FRAME_SIZE_BITS / 8;
    static constexpr int        BURST_FRAME_SYMBOL_NUM          = BURST_FRAME_SIZE_BITS / 2;

    // Burst payload sections
    static constexpr int        BURST_PAYLOAD_SECT_SIZE_BITS    = 108;
    static constexpr int        BURST_PAYLOAD_SECT_SYMBOL_NUM   = BURST_PAYLOAD_SECT_SIZE_BITS / 2;
    static constexpr int        BURST_PAYLOAD_SIZE_BITS         = BURST_PAYLOAD_SECT_SIZE_BITS * 2;
    static constexpr int        BURST_PAYLOAD_SIZE_BYTES        = BURST_PAYLOAD_SIZE_BITS / 8;

    // 'symbol_t' is a float right now but can be changed if necessary
    typedef float symbol_t;
    #define PERCENT_TO_VALUE(x) (x / 100)
}

