#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <fstream>
#include <vector>

#define HAVE_REMOTE
#include <pcap.h>

#include "crc.h"
#include "ethernet.h"
#include "arg_parser.h"

int main(int argc, char* argv[]) {
    ArgParser parser(argc, argv);

    int         dev_index  = 0;
    bool        list_only  = false;
    const char* input_file = "";
    const char* src_mac    = "ff:ff:ff:ff:ff:ff";

    while (!parser.empty()) {
        const char* arg = nullptr;
        if (parser.flag("-list")) {
            list_only = true;
        } else if (parser.named_arg("-dev", arg)) {
            dev_index = atoi(arg) - 1;
        } else if (parser.named_arg("-i", arg)) {
            input_file = arg;
        } else if (parser.named_arg("-src", arg)) {
            src_mac = arg;
        } else {
            parser.ignore_once();
        }
    }

    //! lookup devices
    pcap_if_t* devlist = nullptr;
    char       errbuf[PCAP_ERRBUF_SIZE]{};

    if (pcap_findalldevs(&devlist, errbuf) == -1) {
        perror(errbuf);
        return -1;
    }

    std::vector<pcap_if_t*> devices{};
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
        std::ifstream file(input_file);
        assert(file.is_open());

        //! init crc32 table
        uint32_t crc32_table[256]{};
        init_crc32_table(crc32_table);

        //! build packet
        eth_packet_t* packet =
            (eth_packet_t*)new uint8_t[sizeof(eth_packet_t) + MAX_SIZE];
        eth_init_header(packet, src_mac);
        size_t data_size = eth_load_data(packet, file, crc32_table);

        //! send packet
        assert(devices.size() > dev_index);
        pcap_t* handle = pcap_open_live(
            devices[dev_index]->name, //<! device name
            65535,
            1,
            1000,  //<! timeout
            errbuf //<! error buffer
        );
        assert(handle != nullptr);

        if (pcap_datalink(handle) != DLT_EN10MB) {
            perror("Not a valid ethernet network");
            return -1;
        }

        while (1) {
            auto resp = pcap_sendpacket(
                handle, (uint8_t*)packet, sizeof(eth_header_t) + data_size);
            assert(resp == 0);
        }

        pcap_close(handle);
    }

    pcap_freealldevs(devlist);

    return 0;
}
