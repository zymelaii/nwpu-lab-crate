#include "VMUX2_CA.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <iostream>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VMUX2_CA>();
    dut->trace(trace.get(), 5);
    trace->open("sim_MUX2_CA.vcd");

    vluint64_t simTime = 0;
    for (int i = 0; i < (1 << 3); ++i) {
        dut->sel     = i & 1;
        dut->dat_in1 = (i >> 1) & 1;
        dut->dat_in2 = (i >> 2) & 1;
        dut->eval();
        trace->dump(simTime);

        int expected = dut->sel == 0 ? dut->dat_in1 : dut->dat_in2;
        int actual   = dut->dat_out;
        printf(
            "[%s] sel=%d dat_in1=%d dat_in2=%d expected=%d actual=%d\n",
            actual == expected ? "PASS" : "FAIL",
            dut->sel,
            dut->dat_in1,
            dut->dat_in2,
            expected,
            actual);

        ++simTime;
    }

    trace->close();

    return 0;
}
