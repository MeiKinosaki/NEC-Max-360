#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace pc6001 {

class Z80 {
public:
    struct Registers {
        std::uint8_t a = 0, f = 0;
        std::uint8_t b = 0, c = 0;
        std::uint8_t d = 0, e = 0;
        std::uint8_t h = 0, l = 0;
        std::uint16_t sp = 0xFFFF;
        std::uint16_t pc = 0;
        bool halted = false;
    };

    Z80();

    void reset();
    void load(std::uint16_t address, const std::uint8_t* data, std::size_t size);
    unsigned step();

    std::uint8_t read(std::uint16_t address) const;
    void write(std::uint16_t address, std::uint8_t value);

    Registers regs{};

private:
    std::array<std::uint8_t, 65536> memory_{};
    std::uint16_t fetch16();
    void push16(std::uint16_t value);
    std::uint16_t pop16();
    std::uint8_t fetch8();
    std::uint8_t& reg8(unsigned index);
    std::uint16_t pair(unsigned index) const;
    void setPair(unsigned index, std::uint16_t value);
    void setLogicFlags(std::uint8_t value);
    void alu(unsigned operation, std::uint8_t value);
};

} // namespace pc6001
