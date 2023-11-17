#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <time.h>

#define HAVE_REMOTE
#include <pcap.h>

#include "crc.h"
#include "ethernet.h"
#include "arg_parser.h"

void timeval2date(const timeval &ts, char *buffer, size_t size) {
    time_t time      = ts.tv_sec;
    auto   timestamp = localtime(&time);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timestamp);
}

void eth_recv_job(
    uint8_t *args, const pcap_pkthdr *header, const uint8_t *content) {
    uint32_t crc32_table[256]{};
    init_crc32_table(crc32_table);

    auto packet  = (const eth_packet_t *)content;
    auto data    = packet->data;
    auto datalen = header->len - sizeof(eth_header_t) - 4;
    assert(datalen > 0);
    char buffer[32]{};

    puts("===  pcap header  ===");
    timeval2date(header->ts, buffer, sizeof(buffer));
    printf("timestamp: %s.%ld\n", buffer, header->ts.tv_usec);
    printf("packet size: %d\n", header->len);

    puts("=== packet header ===");
    strmac_s(&packet->header.mac_src, buffer, sizeof(buffer));
    printf("src mac: %s\n", buffer);
    strmac_s(&packet->header.mac_dst, buffer, sizeof(buffer));
    printf("dst mac: %s\n", buffer);
    printf("eth type: %#x\n", packet->header.eth_type);

    puts("=== content ===");
    uint32_t remote_crc = *(uint32_t *)(data + datalen);
    printf("remote crc: %#x\n", remote_crc);
    uint32_t local_crc = compute_crc(data, datalen, crc32_table);
    printf("local crc: %#x\n", local_crc);

    //! write out
    std::ofstream file("recv.dat");
    assert(file.is_open());
    file.write((char *)data, datalen);
}

int main(int argc, char *argv[]) {
    ArgParser parser(argc, argv);

    int  dev_index = 0;
    bool list_only = false;

    while (!parser.empty()) {
        const char *arg = nullptr;
        if (parser.flag("-list")) {
            list_only = true;
        } else if (parser.named_arg("-dev", arg)) {
            dev_index = atoi(arg) - 1;
        } else {
            parser.ignore_once();
        }
    }

    //! mac address
    const char *mac_localhost = PCAP_SRC_IF_STRING;
    char       *mac_from      = new char[strlen(mac_localhost) + 1]{};
    strcpy(mac_from, mac_localhost);

    //! lookup devices
    pcap_if_t *devlist = nullptr;
    char       errbuf[PCAP_ERRBUF_SIZE]{};

    if (pcap_findalldevs_ex(mac_from, nullptr, &devlist, errbuf) == -1) {
        perror(errbuf);
        return -1;
    }

    std::vector<pcap_if_t *> devices{};
    for (auto dev = devlist; dev != nullptr; dev = dev->next) {
        devices.push_back(dev);
    }

    if (list_only) {
        puts("Available devices:");
        for (int i = 0; i < devices.size(); ++i) {
            auto dev = devices[i];
            printf("- [%02d] %s", i + 1, dev->name);
            if (dev->description) { printf(" (%s)\n", dev->description); }
        }

        if (devices.empty()) {
            perror("No devices available");
            return -1;
        }
    } else {
        //! receive packet
        assert(devices.size() > dev_index && "device index out of range");
        pcap_t *handle = pcap_open(
            devices[dev_index]->name,
            65535,
            PCAP_OPENFLAG_PROMISCUOUS,
            5, //<! timeout
            nullptr,
            errbuf //<! error buffer
        );
        assert(handle != nullptr);

        //! accept only once
        pcap_loop(handle, 1, eth_recv_job, nullptr);

        pcap_close(handle);
    }

    pcap_freealldevs(devlist);

    return 0;
}
