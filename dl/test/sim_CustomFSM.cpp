#include "VCustomFSM.h"
#include "VCustomFSM___024root.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <bitset>
#include <set>
#include <random>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VCustomFSM>();
    dut->trace(trace.get(), 5);
    trace->open("sim_CustomFSM.vcd");

    vluint64_t simTime = 0;

    dut->clk = 0;
    dut->rst = 0;
    dut->eval();
    trace->dump(simTime++);

    dut->clk = 1;
    dut->rst = 1;
    dut->eval();
    trace->dump(simTime++);

    dut->rst = 0;
    dut->eval();
    assert(dut->rootp->CustomFSM__DOT__state == 0 && dut->dat_out == 0);

    std::mt19937       rng(std::random_device{}());
    std::map<int, int> stateHit{};
    int                prev_dat = 0;
    while (true) {
        ++stateHit[dut->rootp->CustomFSM__DOT__state];
        dut->dat_in = rng() & 1;
        dut->clk    = !dut->clk;
        dut->eval();
        dut->clk = !dut->clk;
        dut->eval();
        printf(
            "state=%d, next_state=%d, prev_dat=%d, dat_in=%d, dat_out=%d "
            "[%s]\n",
            dut->rootp->CustomFSM__DOT__state,
            dut->rootp->CustomFSM__DOT__next_state,
            prev_dat,
            dut->dat_in,
            dut->dat_out,
            (prev_dat == dut->dat_in) == dut->dat_out ? "PASS" : "FAIL");
        prev_dat = dut->dat_in;
        trace->dump(simTime);
        simTime += 2;
        if (stateHit.size() < 5) { continue; }
        bool shouldQuit = true;
        for (int i = 0; i < 5; ++i) {
            shouldQuit &= stateHit[i] >= (i == 0 ? 1 : 2);
        }
        if (shouldQuit) { break; }
    }

    trace->dump(simTime);
    trace->close();

    return 0;
}
