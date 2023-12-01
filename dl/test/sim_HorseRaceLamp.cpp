#include "VHorseRaceLamp.h"
#include "VHorseRaceLamp___024root.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <bitset>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VHorseRaceLamp>();
    dut->trace(trace.get(), 5);
    trace->open("sim_HorseRaceLamp.vcd");

    vluint64_t simTime = 0;

    dut->clk = 0;
    dut->rst = 1;
    dut->eval();
    trace->dump(simTime++);

    dut->clk = 1;
    dut->rst = 0;
    dut->eval();
    trace->dump(simTime++);

    dut->clk = 0;
    dut->rst = 1;
    dut->eval();
    trace->dump(simTime++);

    for (int i = 0; i < (1 << 24); ++i) {
        dut->clk = !dut->clk;
        dut->eval();
        dut->clk = !dut->clk;
        dut->eval();
        trace->dump(simTime);
        simTime += 2;
        putchar('\r');
        std::cout << std::bitset<25>(dut->rootp->HorseRaceLamp__DOT__count)
                  << " " << std::setw(2)
                  << (dut->rootp->HorseRaceLamp__DOT__count >> 21) << " "
                  << std::flush;
        for (int i = 0; i < 8; ++i) {
            printf(((dut->out >> (7 - i)) & 1) ? "■" : "□");
        }
    }

    trace->dump(simTime);
    trace->close();

    return 0;
}
