#include "VDEC_2_4.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <bitset>
#include <iostream>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VDEC_2_4>();
    dut->trace(trace.get(), 5);
    trace->open("sim_DEC_2_4.vcd");

    vluint64_t simTime = 0;
    int        dat_in  = 0;
    for (int i = 0; i < (1 << 2); ++i) {
        dut->dat_in = i;
        dut->eval();
        trace->dump(simTime);
        auto expected = std::bitset<4>(1 << dut->dat_in);
        auto actual   = std::bitset<4>(dut->dat_out);
        printf("[%02lu] 0x%1x -> ", simTime, dut->dat_in);
        std::cout << actual << ", expect " << expected << " ["
                  << (actual == expected ? "PASS" : "FAIL") << "]" << std::endl;
        ++simTime;
    }

    trace->close();

    return 0;
}
