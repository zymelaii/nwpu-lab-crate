#include "VCOMP_1.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <iostream>
#include <bitset>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VCOMP_1>();
    dut->trace(trace.get(), 5);
    trace->open("sim_COMP_1.vcd");

    vluint64_t simTime = 0;
    for (int i = 0; i < (1 << 2); ++i) {
        dut->lhs = (i >> 0) & 0b1;
        dut->rhs = (i >> 1) & 0b1;
        dut->eval();
        trace->dump(simTime);
        int expected = (dut->lhs == dut->rhs) << 2 | (dut->lhs < dut->rhs) << 1
                     | (dut->lhs > dut->rhs) << 0;
        int actual = dut->eq << 2 | dut->lt << 1 | dut->gt << 0;
        printf(
            "[%02lu] %d <> %d = %d { eq: %d, lt: %d, gt: %d }, expect %d "
            "[%s]\n",
            simTime,
            dut->lhs,
            dut->rhs,
            actual,
            dut->eq,
            dut->lt,
            dut->gt,
            expected,
            actual == expected ? "PASS" : "FAIL");
        ++simTime;
    }

    trace->close();

    return 0;
}
