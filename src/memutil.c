#include <stdint.h>

uint16_t jr_mem_read_uint16be(uint8_t *ptr) {
    uint16_t b0 = (uint16_t)ptr[0];
    uint16_t b1 = (uint16_t)ptr[1];
    return (b0 << 8) | b1;
}

uint32_t jr_mem_read_uint32be(uint8_t *ptr) {
    uint32_t b0 = (uint32_t)ptr[0];
    uint32_t b1 = (uint32_t)ptr[1];
    uint32_t b2 = (uint32_t)ptr[2];
    uint32_t b3 = (uint32_t)ptr[3];
    return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

uint16_t jr_mem_read_uint16le(uint8_t *ptr) {
    uint16_t b0 = (uint16_t)ptr[0];
    uint16_t b1 = (uint16_t)ptr[1];
    return (b1 << 8) | b0;
}

uint32_t jr_mem_read_uint32le(uint8_t *ptr) {
    uint32_t b0 = (uint32_t)ptr[0];
    uint32_t b1 = (uint32_t)ptr[1];
    uint32_t b2 = (uint32_t)ptr[2];
    uint32_t b3 = (uint32_t)ptr[3];
    return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}
