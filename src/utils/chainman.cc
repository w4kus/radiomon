// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

// PLACEHOLDER for the chain manager

/*
    struct block_entry_t
    {
        size_t hash;
        void *obj;
    };

    auto ptr = std::make_unique<dsp::firfilter_ff>(util::make_aligned_ptr<float>(128));

    block_entry_t blks =
    {
        .hash = typeid(dsp::func_ff).hash_code(),
        .obj = ptr.get()
    };

    if (typeid(dsp::func_ff).hash_code() == blks.hash)
    {
        printf("It works\n"); 

        auto b = (dsp::block<dsp::func_ff> *)blks.obj;
    }
*/
