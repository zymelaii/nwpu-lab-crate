#include "VCounter.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <bitset>
#include <iostream>
#include <assert.h>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VCounter>();
    dut->trace(trace.get(), 5);
    trace->open("sim_Counter.vcd");

    vluint64_t simTime = 0;

    dut->clk   = 0;
    dut->rst_n = 1;
    dut->eval();
    trace->dump(simTime);
    simTime += 2;

    for (int i = 1; i <= 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            dut->clk = !dut->clk;
            dut->eval();
            trace->dump(simTime);
            simTime += 2;
        }
        assert(dut->cnt == i);
    }

    dut->clk = !dut->clk;
    dut->eval();
    assert(dut->cnt == 3);
    trace->dump(simTime);
    simTime    += 1;
    dut->rst_n = 0;
    dut->eval();
    trace->dump(simTime);
    simTime    += 1;
    dut->rst_n = 1;
    assert(dut->cnt == 0);

    int expected = 1;
    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 2; ++j) {
            dut->clk = !dut->clk;
            dut->eval();
            trace->dump(simTime);
            simTime += 2;
        }
        assert(dut->cnt == expected);
        expected = (expected + 1) % 16;
    }

    trace->close();

    return 0;
}
