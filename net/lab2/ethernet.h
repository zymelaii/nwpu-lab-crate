#pragma once

#include <stddef.h>
#include <stdint.h>
#include <istream>

#define MAX_SIZE       2048
#define ETHERNET_TYPE  0x80

typedef struct mac_s {
    uint8_t parts[6];
} mac_t;

typedef struct eth_header_s {
    mac_t    mac_dst;
    mac_t    mac_src;
    uint16_t eth_type;
} eth_header_t;

typedef struct eth_packet_s {
    eth_header_t header;
    uint8_t      data[0];
} eth_packet_t;

void eth_init_header(eth_packet_t* packet, const char* src_mac);

size_t eth_load_data(
    eth_packet_t* packet, std::istream& stream, uint32_t (&crc32_table)[256]);

void strmac_s(const mac_t* mac, char* buf, int size);
