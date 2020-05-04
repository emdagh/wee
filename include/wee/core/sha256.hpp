#pragma once

#include <cstdint>

#define SHA256_BLOCK_SIZE   32

namespace wee {
    struct sha256_context {
        uint8_t data[64];
        uint32_t datalen;
        uint64_t bitlen;
        uint32_t state[8];       
    };

    void sha256_init(sha256_context*);
    void sha256_update(sha256_context*, const uint8_t*, std::size_t);
    void sha256_final(sha256_context*, uint8_t*);
}