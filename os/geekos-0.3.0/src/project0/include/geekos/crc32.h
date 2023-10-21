#ifndef GEEKOS_CRC32_H
#define GEEKOS_CRC32_H

#include <stddef.h>
#include <geekos/ktypes.h>

void Init_CRC32(void);
ulong_t crc32(ulong_t crc, char const *buf, size_t len);

#endif /* GEEKOS_CRC32_H */
