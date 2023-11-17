#include "VHALF_ADDER_1.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <iostream>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VHALF_ADDER_1>();
    dut->trace(trace.get(), 5);
    trace->open("sim_HALF_ADDER_1.vcd");

    vluint64_t simTime = 0;
    for (int i = 0; i < (1 << 3); ++i) {
        dut->a = i & 1;
        dut->b = (i >> 1) & 1;
        dut->eval();
        trace->dump(simTime);

        int expected = dut->a + dut->b;
        int actual   = dut->s0 | (dut->s1 << 1);
        printf(
            "[%s] a=%d b=%d s0=%d s1=%d expected=%d actual=%d\n",
            actual == expected ? "PASS" : "FAIL",
            dut->a,
            dut->b,
            dut->s0,
            dut->s1,
            expected,
            actual);

        ++simTime;
    }

    trace->close();

    return 0;
}
