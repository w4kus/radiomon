#pragma once

namespace dmr {
    // 4FSK symbols bits deviation
    //      +3      0b01 +1.944 Khz
    //      +1      0b00 +0.648 KHz
    //      -1      0b10 -0.648 KHz
    //      -3      0b11 -1.944 KHz


    // 4800 baud
    // 1ms = 4.8 sym per msec = 9.6 bpms
    static const int        SAMPLES_PER_SYMBOL              = 5;    // 24KS/s

    // 5ms sync / embedded signal word
    static const int        SYNC_WORD_SIZE_BITS             = 48;
    static const int        SYNC_WORD_SIZE_BYTES            = SYNC_WORD_SIZE_BITS / 8;
    static const int        SYNC_WORD_SYMBOL_NUM            = SYNC_WORD_SIZE_BITS / 2;

    // 2.5ms guard time (MS), CACH (BS)
    static const int        TDMA_CACH_SIZE_BITS             = 24;
    static const int        TDMA_CACH_SIZE_BYTES            = TDMA_CACH_SIZE_BITS / 8;
    static const int        TDMA_CACH_SYMBOL_NUM            = TDMA_CACH_SIZE_BITS / 2;

    // Burst structure
    static const int        BURST_FRAME_SIZE_BITS           = 264 + TDMA_CACH_SIZE_BITS;
    static const int        BURST_FRAME_SIZE_BYTES          = BURST_FRAME_SIZE_BITS / 8;
    static const int        BURST_FRAME_SYMBOL_NUM          = BURST_FRAME_SIZE_BITS / 2;

    // Burst payload sections
    static const int        BURST_PAYLOAD_SECT_SIZE_BITS    = 108;
    static const int        BURST_PAYLOAD_SECT_SYMBOL_NUM   = BURST_PAYLOAD_SECT_SIZE_BITS / 2;
    static const int        BURST_PAYLOAD_SIZE_BITS         = BURST_PAYLOAD_SECT_SIZE_BITS * 2;
    static const int        BURST_PAYLOAD_SIZE_BYTES        = BURST_PAYLOAD_SIZE_BITS / 8;

    // Complete TDMA frame (BS) - CACH bits are sent from a MS (receive during this time, 2.5ms)
    static const int        TDMA_FRAME_SIZE_BITS            = TDMA_CACH_SIZE_BITS 
                                                              + BURST_PAYLOAD_SIZE_BITS
                                                              + SYNC_WORD_SIZE_BITS;
    static const int        TDMA_FRAME_SIZE_BYTES           = TDMA_FRAME_SIZE_BITS / 8;
    static const int        TDMA_FRAME_SYMBOL_NUM           = TDMA_FRAME_SIZE_BITS / 2;

    typedef float symbol_t;

    class error : std::exception
    {
    public:
        error(const char *e, int c = -1) :
            err(e),
            code(c)
        { }

        const char *err;
        int code;
        const char *what() { return err; }
    };
}

