#include "cpu/z80.h"

#include <cstdint>
#include <iostream>

int main() {
    pc6001::Z80 cpu;
    const std::uint8_t smoke_test[] = {0x3E, 0x2A, 0x76}; // LD A,42; HALT
    cpu.load(0x0000, smoke_test, sizeof(smoke_test));

    unsigned steps = 0;
    while (!cpu.regs.halted && steps < 16) {
        cpu.step();
        ++steps;
    }

    std::cout << "NEC PC-6001 core bring-up: A="
              << static_cast<unsigned>(cpu.regs.a)
              << ", steps=" << steps << "\n";
    return cpu.regs.a == 0x2A && cpu.regs.halted ? 0 : 1;
}
