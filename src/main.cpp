#include "machine/machine.h"

#include <cstdint>
#include <iostream>

int main() {
    pc6001::Machine machine;
    const std::uint8_t smokeTest[] = {
        0x3E, 0x2A,             // LD A,42
        0x32, 0x00, 0x20,       // LD (0x2000),A
        0x3A, 0x00, 0x20,       // LD A,(0x2000)
        0x76                    // HALT
    };
    machine.reset();
    machine.memory().load(0x0000, smokeTest, sizeof(smokeTest));

    unsigned steps = 0;
    while (!machine.cpu().regs.halted && steps < 16) {
        machine.step();
        ++steps;
    }
    const bool ok = machine.cpu().regs.a == 0x2A &&
                    machine.memory().read(0x2000) == 0x2A &&
                    machine.cpu().regs.halted;
    std::cout << "NEC Max 360 | PC-6001 shared-bus smoke test: "
              << (ok ? "PASS" : "FAIL")
              << ", steps=" << steps
              << ", cycles=" << machine.totalCycles() << "\n";
    return ok ? 0 : 1;
}
