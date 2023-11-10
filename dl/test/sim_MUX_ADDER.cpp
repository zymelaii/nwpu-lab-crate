#include "VMUX_ADDER.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <iostream>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut = std::make_unique<VMUX_ADDER>();
    dut->trace(trace.get(), 5);
    trace->open("sim_MUX_ADDER.vcd");

    vluint64_t simTime = 0;
    for (int i = 0; i < (1 << 6); ++i) {
        dut->sel = i & 0b1;
        dut->dat_in1 = (i >> 1) & 0b11;
        dut->dat_in2 = (i >> 3) & 0b11;
        dut->eval();
        trace->dump(simTime);

        int expected = dut->sel == 0 ? (dut->dat_in1 & 1) + (dut->dat_in2 & 1) : ((dut->dat_in1 >> 1) & 1) + ((dut->dat_in2 >> 1) & 1);
        printf("[%s] sel=%d, dat_in1=0b%d%d, dat_in2=0b%d%d, expected=%d, actual=%d\n",
            dut->dat_out == expected ? "PASS" : "FAIL",
            dut->sel,
            dut->dat_in1 & 1, (dut->dat_in1 >> 1) & 1,
            dut->dat_in2 & 1, (dut->dat_in2 >> 1) & 1,
            expected,
            dut->dat_out
        );

        ++simTime;
    }

    trace->close();

    return 0;
}
