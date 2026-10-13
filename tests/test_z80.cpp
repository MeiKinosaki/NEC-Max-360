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
    cpu.step(); assert(cpu.regs.pc == static_cast<std::uint16_t>(before + 2));
    cpu.step(); assert(cpu.regs.a == 8);
    cpu.step(); assert(cpu.regs.halted);
}

static void test_inc_dec_flags_and_carry_preservation() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0x3E, 0x7F, // LD A,0x7F
        0x3C,       // INC A -> 0x80, overflow
        0x3D,       // DEC A -> 0x7F, overflow
        0x76
    };
    cpu.load(0, program, sizeof(program));
    cpu.regs.f = 0x01; // INC/DEC must preserve carry.
    assert(cpu.step() == 7 && cpu.regs.a == 0x7F);
    assert(cpu.step() == 4 && cpu.regs.a == 0x80);
    assert(cpu.regs.f == 0x95); // S, H, PV and preserved C.
    assert(cpu.step() == 4 && cpu.regs.a == 0x7F);
    assert(cpu.regs.f == 0x17); // H, PV, N and preserved C.
    assert(cpu.step() == 4 && cpu.regs.halted);
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

static void test_rotate_flags_and_control_instructions() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0x3E, 0x81, // LD A,0x81
        0x07,       // RLCA -> 0x03, carry set
        0x0F,       // RRCA -> 0x81, carry set
        0x2F,       // CPL -> 0x7E, H and N set
        0x37,       // SCF
        0x3F,       // CCF -> carry cleared, H set
        0x06, 0x02, // LD B,2
        0x10, 0xFE, // DJNZ back to itself until B becomes zero
        0x11, 0x34, 0x12, // LD DE,0x1234
        0x21, 0xCD, 0xAB, // LD HL,0xABCD
        0xEB,       // EX DE,HL
        0xF9,       // LD SP,HL
        0xE9        // JP (HL)
    };
    cpu.load(0, program, sizeof(program));
    assert(cpu.step() == 7 && cpu.regs.a == 0x81);
    assert(cpu.step() == 4 && cpu.regs.a == 0x03 && (cpu.regs.f & 1));
    assert(cpu.step() == 4 && cpu.regs.a == 0x81 && (cpu.regs.f & 1));
    assert(cpu.step() == 4 && cpu.regs.a == 0x7E);
    assert((cpu.regs.f & 0x12) == 0x12); // H and N
    assert(cpu.step() == 4 && (cpu.regs.f & 1));
    assert(cpu.step() == 4 && !(cpu.regs.f & 1) && (cpu.regs.f & 0x10));
    assert(cpu.step() == 7 && cpu.regs.b == 2);
    assert(cpu.step() == 13 && cpu.regs.b == 1);
    assert(cpu.step() == 8 && cpu.regs.b == 0);
    assert(cpu.step() == 10 && cpu.regs.d == 0x12 && cpu.regs.e == 0x34);
    assert(cpu.step() == 10 && cpu.regs.h == 0xAB && cpu.regs.l == 0xCD);
    assert(cpu.step() == 4 && cpu.regs.h == 0x12 && cpu.regs.l == 0x34);
    assert(cpu.step() == 6 && cpu.regs.sp == 0x1234);
    assert(cpu.step() == 4 && cpu.regs.pc == 0x1234);
}

int main() {
    test_load_store_and_halt();
    test_register_load_and_arithmetic();
    test_call_return_and_stack();
    test_inc_dec_flags_and_carry_preservation();
    test_rotate_flags_and_control_instructions();
    return 0;
}
