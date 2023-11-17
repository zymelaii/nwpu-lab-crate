#pragma once

#include <stdint.h>
#include <stddef.h>

void     init_crc32_table(uint32_t (&table)[256]);
uint32_t compute_crc(const uint8_t* data, size_t size, uint32_t (&table)[256]);
