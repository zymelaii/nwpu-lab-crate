#include "VTimer.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <bitset>
#include <iostream>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VTimer>();
    dut->trace(trace.get(), 5);
    trace->open("sim_Timer.vcd");

    vluint64_t simTime = 0;

    dut->clk   = 0;
    dut->rst_n = 1;
    dut->eval();
    trace->dump(simTime);
    ++simTime;

    dut->clk   = 0;
    dut->rst_n = 0;
    dut->eval();
    trace->dump(simTime);
    ++simTime;

    dut->clk   = 1;
    dut->rst_n = 0;
    dut->eval();
    trace->dump(simTime);
    ++simTime;

    dut->clk   = 1;
    dut->rst_n = 1;
    dut->eval();
    trace->dump(simTime);
    ++simTime;

    dut->en  = 1;
    dut->clk = 0;
    dut->eval();
    dut->clk = 1;
    dut->eval();
    trace->dump(simTime);
    simTime += 4;
    assert(dut->ready == 0);
    assert(dut->notify == 0);

    auto start = simTime;
    auto end   = start;

    for (int i = 0; i < 4000; ++i) {
        for (int j = 0; j < 10000; ++j) {
            dut->clk = !dut->clk;
            dut->eval();
            if (!dut->ready) { dut->en = 0; }
            if (dut->notify) {
                end = start + i * 10000 + j;
                trace->dump(simTime);
                goto done;
            }
            dut->clk = !dut->clk;
            dut->eval();
        }
        simTime += 10000;
        trace->dump(simTime);
    }
    goto fail;

done:
    std::cout << "timeout: duration: " << (end - start) << std::endl;
    goto cleanup;

fail:
    puts("missing timeout");

cleanup:
    trace->close();

    return 0;
}
