#include "VSerDes.h"

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <iostream>
#include <bitset>

int main(int argc, char *argv[]) {
    Verilated::traceEverOn(true);

    auto trace = std::make_unique<VerilatedVcdC>();
    auto dut   = std::make_unique<VSerDes>();
    dut->trace(trace.get(), 5);
    trace->open("sim_SerDes.vcd");

    vluint64_t simTime = 0;

    //! reset
    dut->clk   = 0;
    dut->rst_n = 1;
    dut->eval();
    dut->rst_n = 0;
    dut->eval();
    dut->rst_n = 1;

    //! start
    dut->clk = 1;
    dut->eval();
    dut->clk = 0;
    dut->eval();

    int input = 0b1101;
    for (int i = 0; i < 5; ++i) {
        dut->clk    = 1;
        dut->ser_in = (input >> (3 - i)) & 1;
        dut->eval();
        trace->dump(++simTime);
        dut->clk = 0;
        dut->eval();
        trace->dump(++simTime);
        std::cout << (int)dut->en_rd << ":" << std::bitset<4>(dut->par_out)
                  << std::endl;
        assert(!dut->en_rd || dut->par_out == (input & 0b1111));
    }

    //! reset
    dut->clk   = 0;
    dut->rst_n = 1;
    dut->eval();
    dut->rst_n = 0;
    dut->eval();
    dut->rst_n = 1;

    //! start
    dut->clk = 1;
    dut->eval();
    dut->clk = 0;
    dut->eval();

    input = 0b0101;
    for (int i = 0; i < 5; ++i) {
        dut->clk    = 1;
        dut->ser_in = (input >> (3 - i)) & 1;
        dut->eval();
        trace->dump(++simTime);
        dut->clk = 0;
        dut->eval();
        trace->dump(++simTime);
        std::cout << (int)dut->en_rd << ":" << std::bitset<4>(dut->par_out)
                  << std::endl;
        assert(!dut->en_rd || dut->par_out == (input & 0b1111));
    }

    trace->close();

    return 0;
}
