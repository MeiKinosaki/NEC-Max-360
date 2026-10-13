#include "cpu/z80.h"

#include <cassert>
#include <cstdint>

int main() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0x3E, 0x2A, // LD A,42
        0x32, 0x00, 0x20, // LD (0x2000),A
        0x3A, 0x00, 0x20, // LD A,(0x2000)
        0x76 // HALT
    };
    cpu.load(0x0000, program, sizeof(program));

    cpu.step();
    assert(cpu.regs.a == 0x2A);
    cpu.step();
    assert(cpu.read(0x2000) == 0x2A);
    cpu.step();
    assert(cpu.regs.a == 0x2A);
    cpu.step();
    assert(cpu.regs.halted);
    return 0;
}
