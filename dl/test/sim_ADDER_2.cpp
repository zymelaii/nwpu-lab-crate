#include "VADDER_2.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <iostream>
#include <bitset>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VADDER_2>();
    dut->trace(trace.get(), 5);
    trace->open("sim_ADDER_2.vcd");

    vluint64_t simTime = 0;
    for (int i = 0; i < (1 << 5); ++i) {
        dut->lhs      = (i >> 0) & 0b11;
        dut->rhs      = (i >> 2) & 0b11;
        dut->carry_in = (i >> 4) & 0b1;
        dut->eval();
        trace->dump(simTime);
        int expected = dut->lhs + dut->rhs + dut->carry_in;
        int actual   = (dut->carry_out << 2) | dut->sum;
        printf(
            "[%02lu] (%d+%d)+%d=%d",
            simTime,
            dut->lhs,
            dut->rhs,
            dut->carry_in,
            actual);
        std::cout << "~0b" << (int)dut->carry_out << ":"
                  << std::bitset<2>(dut->sum);
        printf(
            ", expect %d [%s]\n",
            expected,
            actual == expected ? "PASS" : "FAIL");
        ++simTime;
    }

    trace->close();

    return 0;
}
