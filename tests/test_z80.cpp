#include "cpu/z80.h"

#include <cassert>
#include <cstdint>

static void test_load_store_and_halt() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0x3E, 0x2A,             // LD A,42
        0x32, 0x00, 0x20,       // LD (0x2000),A
        0x3A, 0x00, 0x20,       // LD A,(0x2000)
        0x76                    // HALT
    };
    cpu.load(0, program, sizeof(program));
    assert(cpu.step() == 7 && cpu.regs.a == 0x2A);
    assert(cpu.step() == 13 && cpu.read(0x2000) == 0x2A);
    assert(cpu.step() == 13 && cpu.regs.a == 0x2A);
    assert(cpu.step() == 4 && cpu.regs.halted);
    assert(cpu.step() == 4);
}

static void test_register_load_and_arithmetic() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0x06, 0x03, // LD B,3
        0x3E, 0x04, // LD A,4
        0x80,       // ADD A,B
        0xFE, 0x07, // CP 7
        0x20, 0x02, // JR NZ,+2 (not taken)
        0x3C,       // INC A
        0x76
    };
    cpu.load(0, program, sizeof(program));
    cpu.step(); assert(cpu.regs.b == 3);
    cpu.step(); assert(cpu.regs.a == 4);
    cpu.step(); assert(cpu.regs.a == 7);
    cpu.step(); assert((cpu.regs.f & 0x40) != 0); // CP result is zero
    const auto before = cpu.regs.pc;
    cpu.step(); assert(cpu.regs.pc == static_cast<std::uint16_t>(before + 1));
    cpu.step(); assert(cpu.regs.a == 8);
    cpu.step(); assert(cpu.regs.halted);
}

static void test_call_return_and_stack() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0xCD, 0x06, 0x00, // CALL 0006
        0x76,             // HALT on return
        0x00, 0x00,
        0x3E, 0x55,       // subroutine: LD A,55
        0xC9              // RET
    };
    cpu.load(0, program, sizeof(program));
    cpu.step();
    assert(cpu.regs.pc == 6);
    cpu.step();
    assert(cpu.regs.a == 0x55);
    cpu.step();
    assert(cpu.regs.pc == 3);
    cpu.step();
    assert(cpu.regs.halted);
}

int main() {
    test_load_store_and_halt();
    test_register_load_and_arithmetic();
    test_call_return_and_stack();
    return 0;
}
