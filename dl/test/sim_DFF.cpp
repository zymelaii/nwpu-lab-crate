#include "VDFF.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <assert.h>
#include <iostream>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VDFF>();
    dut->trace(trace.get(), 5);
    trace->open("sim_DFF.vcd");

    vluint64_t simTime = 0;

    dut->clk    = 0;
    dut->dat_in = 1;
    dut->rst_n  = 1;
    dut->eval();
    trace->dump(simTime);
    simTime += 2;

    dut->clk    = 1;
    dut->dat_in = 1;
    dut->eval();
    assert(dut->dat_out == 1);
    trace->dump(simTime);
    simTime += 2;

    dut->clk    = 0;
    dut->dat_in = 0;
    dut->eval();
    assert(dut->dat_out == 1);
    trace->dump(simTime);
    simTime += 2;

    dut->eval();
    assert(dut->dat_out == 1);
    trace->dump(simTime);
    simTime += 2;

    dut->rst_n = 0;
    dut->eval();
    assert(dut->dat_out == 0);
    trace->dump(simTime);
    simTime += 2;

    dut->eval();
    assert(dut->dat_out == 0);
    trace->dump(simTime);
    simTime += 2;

    trace->close();

    return 0;
}
