#include <assert.h>
#include <string.h>

#include "ethernet.h"
#include "crc.h"

void eth_init_header(eth_packet_t* packet) {
    packet->header.mac_dst.parts[0] = 0x5c;
    packet->header.mac_dst.parts[1] = 0x3a;
    packet->header.mac_dst.parts[2] = 0x45;
    packet->header.mac_dst.parts[3] = 0x2e;
    packet->header.mac_dst.parts[4] = 0x39;
    packet->header.mac_dst.parts[5] = 0xcf;

    packet->header.mac_src.parts[0] = 0x2c;
    packet->header.mac_src.parts[1] = 0x6f;
    packet->header.mac_src.parts[2] = 0xc9;
    packet->header.mac_src.parts[3] = 0x36;
    packet->header.mac_src.parts[4] = 0xa2;
    packet->header.mac_src.parts[5] = 0x1d;

    packet->header.eth_type = ETHERNET_TYPE;
}

size_t eth_load_data(
    eth_packet_t* packet, std::istream& stream, uint32_t (&crc32_table)[256]) {
    size_t  size = 0;
    uint8_t tmp[MAX_SIZE], *p = tmp;

    while (true) {
        auto ch = stream.get();
        if (stream.eof()) { break; }
        *p++ = ch;
        ++size;
    }

    // problem: < 46, ADD 0s+1byte;  >1500 LOST
    assert(size >= 46 && size <= 1500);

    uint32_t crc = compute_crc(tmp, size, crc32_table);

    auto buffer = packet->data;
    for (int i = 0; i < size; ++i) { buffer[i] = tmp[i]; }
    *(uint32_t*)(buffer + size) = crc;

    return size + 4;
}

void strmac_s(const mac_t* mac, char* buf, int size) {
    sprintf_s(
        buf,
        size,
        "%02x:%02x:%02x:%02x:%02x:%02x",
        mac->parts[0],
        mac->parts[1],
        mac->parts[2],
        mac->parts[3],
        mac->parts[4],
        mac->parts[5]);
}
