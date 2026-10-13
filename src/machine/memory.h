#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace pc6001 {

// Configurable 64 KiB bus; RAM-backed by default during early bring-up.
class MemoryBus {
public:
    static constexpr std::size_t kAddressSpace = 65536;
    MemoryBus();
    void reset();
    std::uint8_t read(std::uint16_t address) const;
    void write(std::uint16_t address, std::uint8_t value);
    void load(std::uint16_t address, const std::uint8_t* data, std::size_t size);
    enum class RegionType : std::uint8_t { Ram, Rom, Unmapped };
    void mapRegion(std::uint16_t start, std::size_t size, RegionType type, std::uint8_t fill = 0);
    RegionType regionType(std::uint16_t address) const;
private:
    std::array<std::uint8_t, kAddressSpace> bytes_{};
    std::array<RegionType, kAddressSpace> regions_{};
};
} // namespace pc6001
