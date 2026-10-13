#include "machine/machine.h"

#include <cassert>
#include <cstdint>

int main() {
    pc6001::Machine machine;
    const std::uint8_t program[] = {
        0x3E, 0x5A,       // LD A,5A
        0x32, 0x00, 0x40, // LD (4000),A
        0x3E, 0x00,       // LD A,0
        0x3A, 0x00, 0x40, // LD A,(4000)
        0x76              // HALT
    };

    machine.memory().load(0, program, sizeof(program));
    assert(machine.step() == 7);
    assert(machine.cpu().regs.a == 0x5A);
    assert(machine.step() == 13);
    assert(machine.memory().read(0x4000) == 0x5A);
    assert(machine.step() == 7);
    assert(machine.cpu().regs.a == 0);
    assert(machine.step() == 13);
    assert(machine.cpu().regs.a == 0x5A);
    assert(machine.step() == 4);
    assert(machine.cpu().regs.halted);
    assert(machine.totalCycles() == 44);

    machine.reset();
    assert(machine.totalCycles() == 0);
    assert(machine.memory().read(0x4000) == 0);
    assert(machine.cpu().regs.pc == 0);
    assert(!machine.cpu().regs.halted);
    return 0;
}
