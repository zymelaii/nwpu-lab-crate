#include "VMUX2x1.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <iostream>
#include <bitset>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VMUX2x1>();
    dut->trace(trace.get(), 5);
    trace->open("sim_MUX2x1.vcd");

    vluint64_t simTime = 0;
    int        dat_in  = 0;
    for (int i = 0; i < (1 << 2); ++i) {
        for (int j = 0; j < 1; ++j) {
            dut->dat_in = i;
            dut->sel    = j;
            dut->eval();
            trace->dump(simTime);
            auto expected = (dut->dat_in >> dut->sel) & 1;
            printf("[%02lu] ", simTime);
            std::cout << "dat_in: " << std::bitset<2>(dut->dat_in) << " ";
            printf(
                "sel: %d -> %d, expected: %d [%s]\n",
                dut->sel,
                dut->dat_out,
                expected,
                dut->dat_out == expected ? "PASS" : "FAIL");
            ++simTime;
        }
    }

    trace->close();

    return 0;
}
