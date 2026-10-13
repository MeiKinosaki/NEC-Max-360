#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace pc6001 {

// Temporary flat 64 KiB bus for CPU bring-up.
// Future versions will replace regions with model-specific ROM, RAM and MMIO.
class MemoryBus {
public:
    static constexpr std::size_t kAddressSpace = 65536;

    MemoryBus();
    void reset();
    std::uint8_t read(std::uint16_t address) const;
    void write(std::uint16_t address, std::uint8_t value);

private:
    std::array<std::uint8_t, kAddressSpace> bytes_{};
};

} // namespace pc6001
