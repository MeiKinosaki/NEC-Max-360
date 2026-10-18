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

static void test_conditional_jumps_calls_and_returns() {
    {
        pc6001::Z80 cpu;
        const std::uint8_t program[] = {
            0x3E, 0x00,       // LD A,0
            0xB7,             // OR A -> Z set
            0xCA, 0x08, 0x00, // JP Z,0008 (taken)
            0x3E, 0xFF,       // skipped
            0x76              // 0008 HALT
        };
        cpu.load(0, program, sizeof(program));
        assert(cpu.step() == 7 && cpu.regs.a == 0);
        assert(cpu.step() == 4 && (cpu.regs.f & 0x40));
        assert(cpu.step() == 10 && cpu.regs.pc == 8);
        assert(cpu.step() == 4 && cpu.regs.halted);
    }
    {
        pc6001::Z80 cpu;
        const std::uint8_t program[] = {
            0x3E, 0x00,       // LD A,0
            0xB7,             // OR A -> Z
            0xCC, 0x07, 0x00, // CALL Z,0007
            0x76,             // return address 0006: HALT
            0x3E, 0x55,       // 0007 LD A,55
            0xC9              // 000A RET
        };
        cpu.load(0, program, sizeof(program));
        assert(cpu.step() == 7);
        assert(cpu.step() == 4 && (cpu.regs.f & 0x40));
        assert(cpu.step() == 17 && cpu.regs.pc == 7);
        assert(cpu.step() == 7 && cpu.regs.a == 0x55);
        assert(cpu.step() == 10 && cpu.regs.pc == 6);
        assert(cpu.step() == 4 && cpu.regs.halted);
    }
    {
        pc6001::Z80 cpu;
        const std::uint8_t program[] = {
            0x3E, 0x01,       // LD A,1
            0xB7,             // OR A -> Z clear
            0xC4, 0x07, 0x00, // CALL NZ,0007 (taken)
            0x76,             // HALT after return
            0xC0              // 0007 RET NZ
        };
        cpu.load(0, program, sizeof(program));
        cpu.step();
        cpu.step();
        assert((cpu.regs.f & 0x40) == 0);
        assert(cpu.step() == 17 && cpu.regs.pc == 7);
        assert(cpu.step() == 11 && cpu.regs.pc == 6);
        assert(cpu.step() == 4 && cpu.regs.halted);
    }
}

static void test_add_hl_register_pairs_flags() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0x21, 0xFF, 0x0F, // LD HL,0x0FFF
        0x01, 0x01, 0x00, // LD BC,1
        0x09,             // ADD HL,BC -> 0x1000, half carry
        0x21, 0xFF, 0xFF, // LD HL,0xFFFF
        0x09              // ADD HL,BC -> 0x0000, half carry and carry
    };
    cpu.load(0, program, sizeof(program));
    cpu.regs.f = 0xC4; // S, Z and PV must be preserved by ADD HL,rr.
    assert(cpu.step() == 10);
    assert(cpu.step() == 10);
    assert(cpu.step() == 11);
    assert(cpu.regs.h == 0x10 && cpu.regs.l == 0x00);
    assert(cpu.regs.f == 0xD4); // preserved S/Z/PV plus H
    assert(cpu.step() == 10);
    assert(cpu.step() == 11);
    assert(cpu.regs.h == 0x00 && cpu.regs.l == 0x00);
    assert(cpu.regs.f == 0xD5); // preserved S/Z/PV plus H and C
}

static void test_rst_vectors_and_exchange_stack_hl() {
    {
        pc6001::Z80 cpu;
        const std::uint8_t program[] = {
            0xC7, // RST 00H
            0x76  // HALT at return address 0001
        };
        const std::uint8_t vector[] = {0xC9}; // RET at vector 0000
        cpu.load(0, program, sizeof(program));
        cpu.load(0x0000, vector, sizeof(vector));
        // Place RST at 0008 so its vector and opcode do not overlap.
        const std::uint8_t rst[] = {0xC7, 0x76};
        cpu.load(0x0008, rst, sizeof(rst));
        cpu.regs.pc = 0x0008;
        assert(cpu.step() == 11 && cpu.regs.pc == 0x0000);
        assert(cpu.step() == 10 && cpu.regs.pc == 0x0009);
        assert(cpu.step() == 4 && cpu.regs.halted);
    }
    {
        pc6001::Z80 cpu;
        const std::uint8_t program[] = {
            0x21, 0x34, 0x12, // LD HL,1234
            0xE3,             // EX (SP),HL
            0x76
        };
        cpu.load(0, program, sizeof(program));
        cpu.regs.sp = 0x4000;
        cpu.write(0x4000, 0xCD);
        cpu.write(0x4001, 0xAB);
        assert(cpu.step() == 10);
        assert(cpu.step() == 19);
        assert(cpu.regs.h == 0xAB && cpu.regs.l == 0xCD);
        assert(cpu.read(0x4000) == 0x34 && cpu.read(0x4001) == 0x12);
        assert(cpu.step() == 4 && cpu.regs.halted);
    }
}

static void test_ed_word_memory_transfers() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0x01, 0x34, 0x12,       // LD BC,1234
        0xED, 0x43, 0x00, 0x40,// LD (4000),BC
        0x11, 0x00, 0x00,       // LD DE,0
        0xED, 0x5B, 0x00, 0x40,// LD DE,(4000)
        0x21, 0x78, 0x56,       // LD HL,5678
        0xED, 0x63, 0x02, 0x40,// LD (4002),HL
        0x31, 0x00, 0x00,       // LD SP,0
        0xED, 0x7B, 0x02, 0x40,// LD SP,(4002)
        0x76
    };
    cpu.load(0, program, sizeof(program));
    assert(cpu.step() == 10);
    assert(cpu.step() == 20);
    assert(cpu.read(0x4000) == 0x34 && cpu.read(0x4001) == 0x12);
    assert(cpu.step() == 10);
    assert(cpu.step() == 20 && cpu.regs.d == 0x12 && cpu.regs.e == 0x34);
    assert(cpu.step() == 10);
    assert(cpu.step() == 20);
    assert(cpu.read(0x4002) == 0x78 && cpu.read(0x4003) == 0x56);
    assert(cpu.step() == 10);
    assert(cpu.step() == 20 && cpu.regs.sp == 0x5678);
    assert(cpu.step() == 4 && cpu.regs.halted);
}

static void test_ed_adc_sbc_hl() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0x21, 0xFF, 0x0F, // LD HL,0FFF
        0x01, 0x01, 0x00, // LD BC,0001
        0x37,             // SCF
        0xED, 0x4A,       // ADC HL,BC -> 1001
        0x21, 0x00, 0x10, // LD HL,1000
        0x37,             // SCF
        0xED, 0x42        // SBC HL,BC -> 0FFE
    };
    cpu.load(0, program, sizeof(program));
    assert(cpu.step() == 10);
    assert(cpu.step() == 10);
    assert(cpu.step() == 4);
    assert(cpu.step() == 15);
    assert(cpu.regs.h == 0x10 && cpu.regs.l == 0x01);
    assert((cpu.regs.f & 0x10) != 0);
    assert(cpu.step() == 10);
    assert(cpu.step() == 4);
    assert(cpu.step() == 15);
    assert(cpu.regs.h == 0x0F && cpu.regs.l == 0xFE);
    assert((cpu.regs.f & 0x12) == 0x12);
}

static void test_ed_block_transfer_repeat() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0x21, 0x00, 0x40, // LD HL,4000
        0x11, 0x00, 0x50, // LD DE,5000
        0x01, 0x02, 0x00, // LD BC,2
        0xED, 0xB0,       // LDIR
        0x76
    };
    const std::uint8_t source[] = {0xA5, 0x5A};
    cpu.load(0, program, sizeof(program));
    cpu.load(0x4000, source, sizeof(source));
    assert(cpu.step() == 10);
    assert(cpu.step() == 10);
    assert(cpu.step() == 10);
    assert(cpu.step() == 21);
    assert(cpu.read(0x5000) == 0xA5);
    assert(cpu.regs.b == 0 && cpu.regs.c == 1);
    assert(cpu.regs.h == 0x40 && cpu.regs.l == 0x01);
    assert(cpu.regs.d == 0x50 && cpu.regs.e == 0x01);
    assert((cpu.regs.f & 0x04) != 0);
    assert(cpu.step() == 16);
    assert(cpu.read(0x5001) == 0x5A);
    assert(cpu.regs.b == 0 && cpu.regs.c == 0);
    assert((cpu.regs.f & 0x04) == 0);
    assert(cpu.step() == 4 && cpu.regs.halted);
}

static void test_even_parity_flags() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0xAF,             // XOR A -> A=0, even parity and Z
        0x3E, 0x01,       // LD A,1
        0xB7,             // OR A -> odd parity, Z clear
        0x3E, 0x03,       // LD A,3
        0xB7              // OR A -> two set bits, even parity
    };
    cpu.load(0, program, sizeof(program));
    assert(cpu.step() == 4);
    assert((cpu.regs.f & 0x44) == 0x44); // Z and PV for zero.
    assert(cpu.step() == 7);
    assert(cpu.step() == 4);
    assert((cpu.regs.f & 0x44) == 0); // 1 has odd parity and is nonzero.
    assert(cpu.step() == 7);
    assert(cpu.step() == 4);
    assert((cpu.regs.f & 0x04) != 0); // 3 has two set bits.
    assert((cpu.regs.f & 0x40) == 0);
}

static void test_undocumented_flag_bits() {
    pc6001::Z80 cpu;
    const std::uint8_t program[] = {
        0x3E, 0x28, // LD A,28
        0xB7,       // OR A: bits 5 and 3 follow result
        0xFE, 0x20  // CP 20: undocumented bits follow operand (20)
    };
    cpu.load(0, program, sizeof(program));
    assert(cpu.step() == 7);
    assert(cpu.step() == 4);
    assert((cpu.regs.f & 0x28) == 0x28);
    assert(cpu.step() == 7);
    assert((cpu.regs.f & 0x28) == 0x20);
}

static void test_daa_decimal_adjust() {
    {
        pc6001::Z80 cpu;
        const std::uint8_t program[] = {
            0x3E, 0x15, // LD A,15
            0xC6, 0x27, // ADD A,27 -> 3C
            0x27,       // DAA -> 42
            0x76
        };
        cpu.load(0, program, sizeof(program));
        assert(cpu.step() == 7);
        assert(cpu.step() == 7 && cpu.regs.a == 0x3C);
        assert(cpu.step() == 4 && cpu.regs.a == 0x42);
        assert((cpu.regs.f & 0x04) != 0); // 0x42 has even parity.
        assert((cpu.regs.f & 0x01) == 0);
        assert(cpu.step() == 4 && cpu.regs.halted);
    }
    {
        pc6001::Z80 cpu;
        const std::uint8_t program[] = {
            0x3E, 0x99, // LD A,99
            0xC6, 0x01, // ADD A,01 -> 9A
            0x27        // DAA -> 00 with carry
        };
        cpu.load(0, program, sizeof(program));
        assert(cpu.step() == 7);
        assert(cpu.step() == 7 && cpu.regs.a == 0x9A);
        assert(cpu.step() == 4 && cpu.regs.a == 0x00);
        assert((cpu.regs.f & 0x41) == 0x41); // Zero and carry.
    }
    {
        pc6001::Z80 cpu;
        const std::uint8_t program[] = {
            0x3E, 0x45, // LD A,45
            0xD6, 0x06, // SUB 06 -> 3F, half borrow
            0x27        // DAA -> 39, subtraction flags retained
        };
        cpu.load(0, program, sizeof(program));
        assert(cpu.step() == 7);
        assert(cpu.step() == 7 && cpu.regs.a == 0x3F);
        assert(cpu.step() == 4 && cpu.regs.a == 0x39);
        assert((cpu.regs.f & 0x02) != 0); // N is preserved.
        assert((cpu.regs.f & 0x01) == 0); // No borrow from the full byte.
    }
}

int main() {
    test_load_store_and_halt();
    test_register_load_and_arithmetic();
    test_call_return_and_stack();
    test_inc_dec_flags_and_carry_preservation();
    test_rotate_flags_and_control_instructions();
    test_conditional_jumps_calls_and_returns();
    test_add_hl_register_pairs_flags();
    test_rst_vectors_and_exchange_stack_hl();
    test_ed_word_memory_transfers();
    test_ed_adc_sbc_hl();
    test_ed_block_transfer_repeat();
    test_even_parity_flags();
    test_undocumented_flag_bits();
    test_daa_decimal_adjust();
    return 0;
}
