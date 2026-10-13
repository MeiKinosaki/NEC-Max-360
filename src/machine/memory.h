#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace pc6001 {

// 64 KiB temporary flat memory. Not the final PC-6001 memory map.
class MemoryBus {
public:
    static constexpr std::size_t kAddressSpace = 65536;
    MemoryBus();
    void reset();
    std::uint8_t read(std::uint16_t address) const;
    void write(std::uint16_t address, std::uint8_t value);
    void load(std::uint16_t address, const std::uint8_t* data, std::size_t size);
private:
    std::array<std::uint8_t, kAddressSpace> bytes_{};
};
} // namespace pc6001
