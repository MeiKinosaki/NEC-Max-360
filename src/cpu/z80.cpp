#include "cpu/z80.h"

#include <algorithm>
#include <cstddef>

namespace pc6001 {

Z80::Z80() { reset(); }

void Z80::reset() {
    regs = Registers{};
    memory_.fill(0);
}

void Z80::load(std::uint16_t address, const std::uint8_t* data, std::size_t size) {
    if (data == nullptr || size == 0) return;
    const std::size_t start = address;
    const std::size_t count = std::min(size, memory_.size() - start);
    std::copy_n(data, count, memory_.begin() + start);
}

std::uint8_t Z80::read(std::uint16_t address) const {
    return memory_[address];
}

void Z80::write(std::uint16_t address, std::uint8_t value) {
    memory_[address] = value;
}

std::uint16_t Z80::fetch16() {
    const std::uint8_t low = read(regs.pc++);
    const std::uint8_t high = read(regs.pc++);
    return static_cast<std::uint16_t>(low | (static_cast<std::uint16_t>(high) << 8));
}

// Returns a rough base-cycle count for the implemented subset.
// This is a development stub, not a complete or cycle-accurate Z80.
unsigned Z80::step() {
    if (regs.halted) return 4;

    const std::uint8_t opcode = read(regs.pc++);
    switch (opcode) {
    case 0x00: // NOP
        return 4;
    case 0x3E: // LD A,n
        regs.a = read(regs.pc++);
        return 7;
    case 0x06: // LD B,n
        regs.b = read(regs.pc++);
        return 7;
    case 0x0E: // LD C,n
        regs.c = read(regs.pc++);
        return 7;
    case 0x16: // LD D,n
        regs.d = read(regs.pc++);
        return 7;
    case 0x1E: // LD E,n
        regs.e = read(regs.pc++);
        return 7;
    case 0x26: // LD H,n
        regs.h = read(regs.pc++);
        return 7;
    case 0x2E: // LD L,n
        regs.l = read(regs.pc++);
        return 7;
    case 0x32: { // LD (nn),A
        const auto address = fetch16();
        write(address, regs.a);
        return 13;
    }
    case 0x3A: { // LD A,(nn)
        const auto address = fetch16();
        regs.a = read(address);
        return 13;
    }
    case 0xAF: // XOR A
        regs.a = 0;
        regs.f = 0x40; // Z flag; other flag details are not yet modeled.
        return 4;
    case 0xC3: // JP nn
        regs.pc = fetch16();
        return 10;
    case 0x18: { // JR e
        const auto displacement = static_cast<std::int8_t>(read(regs.pc++));
        regs.pc = static_cast<std::uint16_t>(regs.pc + displacement);
        return 12;
    }
    case 0x76: // HALT
        regs.halted = true;
        return 4;
    default:
        // Unknown instructions are treated as NOP only to keep bring-up
        // deterministic. Full opcode coverage and diagnostics are future work.
        return 4;
    }
}

} // namespace pc6001
