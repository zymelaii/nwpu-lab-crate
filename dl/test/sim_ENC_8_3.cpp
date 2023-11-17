#include "VENC_8_3.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <bitset>
#include <iostream>

bool is_valid_input(int i) {
    if (i < 0 || i > 0b10000000) { return false; }
    if (i & (i - 1)) { return false; }
    return true;
}

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VENC_8_3>();
    dut->trace(trace.get(), 5);
    trace->open("sim_ENC_8_3.vcd");

    vluint64_t simTime = 0;
    int        dat_in  = 0;
    for (int i = 0; i < 8; ++i) {
        dut->dat_in = 1 << i;
        dut->eval();
        trace->dump(simTime);
        auto expected = i;
        auto actual   = dut->dat_out;
        printf("[%03lu] ", simTime);
        std::cout << std::bitset<8>(dut->dat_in) << " -> " << (int)actual
                  << ", err: " << (int)dut->err << ", expect " << expected
                  << " [" << (actual == expected ? "PASS" : "FAIL") << "]"
                  << std::endl;
        ++simTime;
    }

    for (int i = 0; i < (1 << 8); ++i) {
        if (is_valid_input(i)) { continue; }
        dut->dat_in = i;
        dut->eval();
        trace->dump(simTime);
        auto expected = 0;
        auto actual   = dut->dat_out;
        printf("[%03lu] ", simTime);
        std::cout << std::bitset<8>(dut->dat_in) << " -> " << (int)actual
                  << ", err: " << (int)dut->err << ", expect " << expected
                  << " [" << (actual == expected ? "PASS" : "FAIL") << "]"
                  << std::endl;
        ++simTime;
    }

    trace->close();

    return 0;
}
