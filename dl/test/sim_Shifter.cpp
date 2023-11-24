#include "VShifter.h"
#include "VShifter___024root.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <assert.h>
#include <iostream>
#include <bitset>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VShifter>();
    dut->trace(trace.get(), 5);
    trace->open("sim_Shifter.vcd");

    vluint64_t simTime = 0;

    dut->clk   = 0;
    dut->rst_n = 1;
    dut->eval();

    //! async load
    int load_val = 0b1101;
    dut->dat_in  = load_val;
    dut->rst_n   = 0;
    dut->eval();
    assert(dut->rootp->Shifter__DOT__data == load_val);
    trace->dump(simTime);

    int value = 0b110101101111;
    std::cout << std::bitset<16>(((load_val << 12) | value)) << std::endl;
    std::cout << std::bitset<4>(dut->rootp->Shifter__DOT__data) << std::endl;

    dut->rst_n = 1;
    for (int i = 0; i < 12; ++i) {
        dut->clk    = !dut->clk;
        dut->ser_in = (value >> (11 - i)) & 1;
        dut->eval();
        trace->dump(++simTime);
        dut->clk = !dut->clk;
        dut->eval();
        trace->dump(++simTime);
        std::cout << std::bitset<16>(((load_val << 12) | value)) << std::endl;
        printf("%*s", i + 1, "");
        std::cout << std::bitset<4>(dut->rootp->Shifter__DOT__data)
                  << std::endl;
        assert(dut->ser_out == (((load_val << 12) | value) >> (14 - i) & 1));
    }

    trace->close();

    return 0;
}
