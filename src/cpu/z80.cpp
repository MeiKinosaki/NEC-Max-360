#include "cpu/z80.h"

#include <algorithm>
#include <cstddef>
#include <utility>

namespace pc6001 {
namespace {
constexpr std::uint8_t FLAG_S = 0x80;
constexpr std::uint8_t FLAG_Z = 0x40;
constexpr std::uint8_t FLAG_H = 0x10;
constexpr std::uint8_t FLAG_PV = 0x04;
constexpr std::uint8_t FLAG_N = 0x02;
constexpr std::uint8_t FLAG_C = 0x01;
bool parityEven(std::uint8_t v) {
    v ^= static_cast<std::uint8_t>(v >> 4);
    v &= 0x0F;
    return ((0x6996u >> v) & 1u) == 0;
}
}

Z80::Z80() { reset(); }

void Z80::reset() {
    regs = Registers{};
    memory_.fill(0);
}

void Z80::load(std::uint16_t address, const std::uint8_t* data, std::size_t size) {
    if (data == nullptr || size == 0) return;
    const std::size_t start = address;
    const std::size_t count = std::min(size, memory_.size() - start);
    if (busWrite_) {
        for (std::size_t i = 0; i < count; ++i)
            busWrite_(static_cast<std::uint16_t>(start + i), data[i]);
    } else {
        std::copy_n(data, count, memory_.begin() + start);
    }
}

void Z80::setBus(std::function<std::uint8_t(std::uint16_t)> readFn,
                 std::function<void(std::uint16_t, std::uint8_t)> writeFn) {
    busRead_ = std::move(readFn);
    busWrite_ = std::move(writeFn);
}
std::uint8_t Z80::read(std::uint16_t address) const {
    return busRead_ ? busRead_(address) : memory_[address];
}
void Z80::write(std::uint16_t address, std::uint8_t value) {
    if (busWrite_) busWrite_(address, value);
    else memory_[address] = value;
}
std::uint8_t Z80::fetch8() { return read(regs.pc++); }

std::uint16_t Z80::fetch16() {
    const std::uint8_t low = fetch8();
    const std::uint8_t high = fetch8();
    return static_cast<std::uint16_t>(low | (static_cast<std::uint16_t>(high) << 8));
}

void Z80::push16(std::uint16_t value) {
    write(--regs.sp, static_cast<std::uint8_t>(value >> 8));
    write(--regs.sp, static_cast<std::uint8_t>(value & 0xFF));
}
std::uint16_t Z80::pop16() {
    const auto low = read(regs.sp++);
    const auto high = read(regs.sp++);
    return static_cast<std::uint16_t>(low | (static_cast<std::uint16_t>(high) << 8));
}

std::uint8_t& Z80::reg8(unsigned index) {
    switch (index & 7u) {
    case 0: return regs.b;
    case 1: return regs.c;
    case 2: return regs.d;
    case 3: return regs.e;
    case 4: return regs.h;
    case 5: return regs.l;
    case 7: return regs.a;
    default: return regs.a; // (HL) is handled separately by the decoder.
    }
}
std::uint16_t Z80::pair(unsigned index) const {
    switch (index & 3u) {
    case 0: return static_cast<std::uint16_t>((regs.b << 8) | regs.c);
    case 1: return static_cast<std::uint16_t>((regs.d << 8) | regs.e);
    case 2: return static_cast<std::uint16_t>((regs.h << 8) | regs.l);
    default: return regs.sp;
    }
}
void Z80::setPair(unsigned index, std::uint16_t value) {
    switch (index & 3u) {
    case 0: regs.b = value >> 8; regs.c = value & 0xFF; break;
    case 1: regs.d = value >> 8; regs.e = value & 0xFF; break;
    case 2: regs.h = value >> 8; regs.l = value & 0xFF; break;
    default: regs.sp = value; break;
    }
}
void Z80::setLogicFlags(std::uint8_t value) {
    regs.f = static_cast<std::uint8_t>((value & FLAG_S) |
        (value == 0 ? FLAG_Z : 0) | (parityEven(value) ? FLAG_PV : 0));
}
void Z80::alu(unsigned operation, std::uint8_t value) {
    const std::uint8_t old = regs.a;
    const unsigned carryIn = (operation == 1 || operation == 3) && (regs.f & FLAG_C) ? 1u : 0u;
    unsigned result = old;
    switch (operation & 7u) {
    case 0: case 1: { // ADD/ADC
        result = static_cast<unsigned>(old) + value + carryIn;
        const auto out = static_cast<std::uint8_t>(result);
        regs.f = static_cast<std::uint8_t>((out & FLAG_S) | (out == 0 ? FLAG_Z : 0) |
            (((old ^ value ^ out) & 0x10) ? FLAG_H : 0) |
            ((~(old ^ value) & (old ^ out) & 0x80) ? FLAG_PV : 0) |
            (result > 0xFF ? FLAG_C : 0));
        regs.a = out; break;
    }
    case 2: case 3: case 7: { // SUB/SBC/CP
        const unsigned sub = static_cast<unsigned>(value) + carryIn;
        const auto out = static_cast<std::uint8_t>(static_cast<unsigned>(old) - sub);
        regs.f = static_cast<std::uint8_t>(FLAG_N | (out & FLAG_S) |
            (out == 0 ? FLAG_Z : 0) | (((old ^ value ^ out) & 0x10) ? FLAG_H : 0) |
            (((old ^ value) & (old ^ out) & 0x80) ? FLAG_PV : 0) |
            (static_cast<unsigned>(old) < sub ? FLAG_C : 0));
        if ((operation & 7u) != 7u) regs.a = out;
        break;
    }
    case 4: regs.a = static_cast<std::uint8_t>(old & value); setLogicFlags(regs.a); regs.f |= FLAG_H; break;
    case 5: regs.a = static_cast<std::uint8_t>(old ^ value); setLogicFlags(regs.a); break;
    case 6: regs.a = static_cast<std::uint8_t>(old | value); setLogicFlags(regs.a); break;
    }
}

unsigned Z80::step() {
    if (regs.halted) return 4;
    const std::uint8_t opcode = fetch8();

    // LD r,r' matrix, including (HL).
    if (opcode >= 0x40 && opcode <= 0x7F) {
        if (opcode == 0x76) { regs.halted = true; return 4; }
        const unsigned dst = (opcode >> 3) & 7u, src = opcode & 7u;
        const std::uint8_t value = src == 6 ? read(pair(2)) : reg8(src);
        if (dst == 6) write(pair(2), value); else reg8(dst) = value;
        return (dst == 6 || src == 6) ? 7 : 4;
    }
    // Register ALU matrix: ADD, ADC, SUB, SBC, AND, XOR, OR, CP.
    if (opcode >= 0x80 && opcode <= 0xBF) {
        const unsigned operation = (opcode >> 3) & 7u, src = opcode & 7u;
        alu(operation, src == 6 ? read(pair(2)) : reg8(src));
        return src == 6 ? 7 : 4;
    }

    // INC r and DEC r.
    if ((opcode & 0xC7) == 0x04 || (opcode & 0xC7) == 0x05) {
        const unsigned index = (opcode >> 3) & 7u;
        const bool decrement = (opcode & 1u) != 0;
        const std::uint8_t old = index == 6 ? read(pair(2)) : reg8(index);
        const std::uint8_t value = static_cast<std::uint8_t>(old + (decrement ? -1 : 1));
        const auto carry = static_cast<std::uint8_t>(regs.f & FLAG_C);
        regs.f = static_cast<std::uint8_t>(carry | (value & FLAG_S) |
            (value == 0 ? FLAG_Z : 0) |
            ((decrement ? (old & 0x0F) == 0 : (old & 0x0F) == 0x0F) ? FLAG_H : 0) |
            ((decrement ? old == 0x80 : old == 0x7F) ? FLAG_PV : 0) |
            (decrement ? FLAG_N : 0));
        if (index == 6) write(pair(2), value); else reg8(index) = value;
        return index == 6 ? 11 : 4;
    }

    const auto conditionMet = [this](unsigned condition) {
        const bool zero = (regs.f & FLAG_Z) != 0;
        const bool carry = (regs.f & FLAG_C) != 0;
        const bool parity = (regs.f & FLAG_PV) != 0;
        const bool sign = (regs.f & FLAG_S) != 0;
        switch (condition & 7u) {
        case 0: return !zero;  // NZ
        case 1: return zero;   // Z
        case 2: return !carry; // NC
        case 3: return carry;  // C
        case 4: return !parity;// PO
        case 5: return parity; // PE
        case 6: return !sign;  // P
        default: return sign;  // M
        }
    };

    switch (opcode) {
    case 0x00: return 4; // NOP
    case 0x07: { // RLCA
        const auto carry = static_cast<std::uint8_t>(regs.a >> 7);
        regs.a = static_cast<std::uint8_t>((regs.a << 1) | carry);
        regs.f = static_cast<std::uint8_t>((regs.f & (FLAG_S | FLAG_Z | FLAG_PV)) |
            (regs.a & 0x28) | carry);
        return 4;
    }
    case 0x0F: { // RRCA
        const auto carry = static_cast<std::uint8_t>(regs.a & 1);
        regs.a = static_cast<std::uint8_t>((regs.a >> 1) | (carry << 7));
        regs.f = static_cast<std::uint8_t>((regs.f & (FLAG_S | FLAG_Z | FLAG_PV)) |
            (regs.a & 0x28) | carry);
        return 4;
    }
    case 0x10: { // DJNZ e
        const auto displacement = static_cast<std::int8_t>(fetch8());
        --regs.b;
        if (regs.b != 0) {
            regs.pc = static_cast<std::uint16_t>(regs.pc + displacement);
            return 13;
        }
        return 8;
    }
    case 0x17: { // RLA
        const auto oldCarry = static_cast<std::uint8_t>(regs.f & FLAG_C);
        const auto carry = static_cast<std::uint8_t>(regs.a >> 7);
        regs.a = static_cast<std::uint8_t>((regs.a << 1) | oldCarry);
        regs.f = static_cast<std::uint8_t>((regs.f & (FLAG_S | FLAG_Z | FLAG_PV)) |
            (regs.a & 0x28) | carry);
        return 4;
    }
    case 0x1F: { // RRA
        const auto oldCarry = static_cast<std::uint8_t>((regs.f & FLAG_C) << 7);
        const auto carry = static_cast<std::uint8_t>(regs.a & 1);
        regs.a = static_cast<std::uint8_t>((regs.a >> 1) | oldCarry);
        regs.f = static_cast<std::uint8_t>((regs.f & (FLAG_S | FLAG_Z | FLAG_PV)) |
            (regs.a & 0x28) | carry);
        return 4;
    }
    case 0x2F: // CPL
        regs.a = static_cast<std::uint8_t>(~regs.a);
        regs.f = static_cast<std::uint8_t>((regs.f & (FLAG_S | FLAG_Z | FLAG_PV | FLAG_C)) |
            FLAG_H | FLAG_N | (regs.a & 0x28));
        return 4;
    case 0x37: // SCF
        regs.f = static_cast<std::uint8_t>((regs.f & (FLAG_S | FLAG_Z | FLAG_PV)) |
            (regs.a & 0x28) | FLAG_C);
        return 4;
    case 0x3F: { // CCF
        const bool oldCarry = (regs.f & FLAG_C) != 0;
        regs.f = static_cast<std::uint8_t>((regs.f & (FLAG_S | FLAG_Z | FLAG_PV)) |
            (regs.a & 0x28) | (oldCarry ? FLAG_H : FLAG_C));
        return 4;
    }
    case 0xEB: { // EX DE,HL
        const auto de = pair(1), hl = pair(2);
        setPair(1, hl); setPair(2, de);
        return 4;
    }
    case 0xE9: regs.pc = pair(2); return 4; // JP (HL)
    case 0xF9: regs.sp = pair(2); return 6; // LD SP,HL
    case 0x01: case 0x11: case 0x21: case 0x31: // LD rr,nn
        setPair((opcode >> 4) & 3u, fetch16()); return 10;
    case 0x02: write(pair(0), regs.a); return 7;
    case 0x12: write(pair(1), regs.a); return 7;
    case 0x0A: regs.a = read(pair(0)); return 7;
    case 0x1A: regs.a = read(pair(1)); return 7;
    case 0x03: case 0x13: case 0x23: case 0x33: {
        const unsigned p = (opcode >> 4) & 3u;
        setPair(p, static_cast<std::uint16_t>(pair(p) + 1)); return 6;
    }
    case 0x0B: case 0x1B: case 0x2B: case 0x3B: {
        const unsigned p = (opcode >> 4) & 3u;
        setPair(p, static_cast<std::uint16_t>(pair(p) - 1)); return 6;
    }
    case 0x3E: regs.a = fetch8(); return 7;
    case 0x06: regs.b = fetch8(); return 7;
    case 0x0E: regs.c = fetch8(); return 7;
    case 0x16: regs.d = fetch8(); return 7;
    case 0x1E: regs.e = fetch8(); return 7;
    case 0x26: regs.h = fetch8(); return 7;
    case 0x2E: regs.l = fetch8(); return 7;
    case 0x36: write(pair(2), fetch8()); return 10;
    case 0x22: { const auto a = fetch16(); write(a, regs.l); write(static_cast<std::uint16_t>(a+1), regs.h); return 16; }
    case 0x2A: { const auto a = fetch16(); regs.l = read(a); regs.h = read(static_cast<std::uint16_t>(a+1)); return 16; }
    case 0x32: { const auto a = fetch16(); write(a, regs.a); return 13; }
    case 0x3A: { const auto a = fetch16(); regs.a = read(a); return 13; }
    case 0xAF: regs.a = 0; setLogicFlags(regs.a); return 4;
    case 0xC2: case 0xCA: case 0xD2: case 0xDA:
    case 0xE2: case 0xEA: case 0xF2: case 0xFA: { // JP cc,nn
        const target = fetch16();
        if (conditionMet((opcode >> 3) & 7u)) regs.pc = target;
        return 10;
    }
    case 0xC4: case 0xCC: case 0xD4: case 0xDC:
    case 0xE4: case 0xEC: case 0xF4: case 0xFC: { // CALL cc,nn
        const target = fetch16();
        if (conditionMet((opcode >> 3) & 7u)) {
            push16(regs.pc);
            regs.pc = target;
            return 17;
        }
        return 10;
    }
    case 0xC0: case 0xC8: case 0xD0: case 0xD8:
    case 0xE0: case 0xE8: case 0xF0: case 0xF8: // RET cc
        if (conditionMet((opcode >> 3) & 7u)) {
            regs.pc = pop16();
            return 11;
        }
        return 5;
    case 0xC3: regs.pc = fetch16(); return 10;
    case 0x18: { const auto d = static_cast<std::int8_t>(fetch8()); regs.pc = static_cast<std::uint16_t>(regs.pc + d); return 12; }
    case 0x20: case 0x28: case 0x30: case 0x38: {
        const auto d = static_cast<std::int8_t>(fetch8());
        const bool z = (regs.f & FLAG_Z) != 0, c = (regs.f & FLAG_C) != 0;
        const unsigned cond = (opcode >> 3) & 3u;
        const bool take = cond == 0 ? !z : cond == 1 ? z : cond == 2 ? !c : c;
        if (take) { regs.pc = static_cast<std::uint16_t>(regs.pc + d); return 12; }
        return 7;
    }
    case 0xCD: { const auto target = fetch16(); push16(regs.pc); regs.pc = target; return 17; }
    case 0xC9: regs.pc = pop16(); return 10;
    case 0xC5: case 0xD5: case 0xE5: case 0xF5: {
        const unsigned p = (opcode >> 4) & 3u;
        const std::uint16_t v = p == 3 ? static_cast<std::uint16_t>((regs.a << 8) | regs.f) : pair(p);
        push16(v); return 11;
    }
    case 0xC1: case 0xD1: case 0xE1: case 0xF1: {
        const unsigned p = (opcode >> 4) & 3u; const auto v = pop16();
        if (p == 3) { regs.a = v >> 8; regs.f = v & 0xFF; } else setPair(p, v);
        return 10;
    }
    case 0xC6: alu(0, fetch8()); return 7;
    case 0xCE: alu(1, fetch8()); return 7;
    case 0xD6: alu(2, fetch8()); return 7;
    case 0xDE: alu(3, fetch8()); return 7;
    case 0xE6: alu(4, fetch8()); return 7;
    case 0xEE: alu(5, fetch8()); return 7;
    case 0xF6: alu(6, fetch8()); return 7;
    case 0xFE: alu(7, fetch8()); return 7;
    case 0xF3: case 0xFB: return 4; // DI/EI: interrupt controller not yet connected.
    case 0x76: regs.halted = true; return 4;
    default: return 4; // Unsupported opcode: placeholder behavior; full core remains incomplete.
    }
}

} // namespace pc6001
