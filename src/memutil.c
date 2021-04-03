#include <stdint.h>

uint32_t jr_mem_read_uint(const uint8_t *ptr, int bytes, int big_endian) {
    if (bytes < 1 || bytes > 4) {
        return 0;
    }
    
    const int shift = big_endian ? 0 : ((bytes - 1) * 8);
    
    uint32_t out = 0;
    for (int j = 0; j < bytes; ++j) {
        if (big_endian) {
            out <<= 8;
        } else {
            out >>= 8;
        }
        out |= ((uint32_t)ptr[j]) << shift;
    }

    return out;
}

uint16_t jr_mem_read_uint16be(const uint8_t *ptr) {
    uint16_t b0 = (uint16_t)ptr[0];
    uint16_t b1 = (uint16_t)ptr[1];
    return (b0 << 8) | b1;
}

uint32_t jr_mem_read_uint32be(const uint8_t *ptr) {
    uint32_t b0 = (uint32_t)ptr[0];
    uint32_t b1 = (uint32_t)ptr[1];
    uint32_t b2 = (uint32_t)ptr[2];
    uint32_t b3 = (uint32_t)ptr[3];
    return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

uint16_t jr_mem_read_uint16le(const uint8_t *ptr) {
    uint16_t b0 = (uint16_t)ptr[0];
    uint16_t b1 = (uint16_t)ptr[1];
    return (b1 << 8) | b0;
}

uint32_t jr_mem_read_uint32le(const uint8_t *ptr) {
    uint32_t b0 = (uint32_t)ptr[0];
    uint32_t b1 = (uint32_t)ptr[1];
    uint32_t b2 = (uint32_t)ptr[2];
    uint32_t b3 = (uint32_t)ptr[3];
    return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}
