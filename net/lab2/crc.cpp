#include <stddef.h>

#include "crc.h"

void init_crc32_table(uint32_t (&table)[256]) {
    uint32_t crc = 0;
    for (int i = 0; i < 256; ++i) {
        crc = i;
        for (int j = 0; j < 8; ++j) {
            crc >>= 1;
            if (crc & 1) { crc ^= 0xedb88320; }
        }
        table[i] = crc;
    }
}

uint32_t compute_crc(const uint8_t* data, size_t size, uint32_t (&table)[256]) {
    uint32_t crc = 0xffffffff;
    for (int i = 0; i < size; ++i) {
        crc = (crc >> 8) ^ table[(crc & 0xFF) ^ data[i]];
    }
    crc ^= 0xffffffff;
    return crc;
}
