#include "machine/memory.h"

#include <algorithm>

namespace pc6001 {
MemoryBus::MemoryBus() { reset(); }

void MemoryBus::reset() {
    bytes_.fill(0);
    regions_.fill(RegionType::Ram);
}

void MemoryBus::configureBasePc6001Map() {
    // This models the baseline machine without a cartridge/expansion device.
    // ROM is initialized to 0xFF until a valid BASIC ROM image is loaded.
    mapRegion(0x0000, 0x4000, RegionType::Rom, 0xFF);
    mapRegion(0x4000, 0x8000, RegionType::Unmapped, 0xFF);
    mapRegion(0xC000, 0x4000, RegionType::Ram, 0x00);
}

std::uint8_t MemoryBus::read(std::uint16_t address) const {
    if (regions_[address] == RegionType::Unmapped) return 0xFF;
    return bytes_[address];
}

void MemoryBus::write(std::uint16_t address, std::uint8_t value) {
    if (regions_[address] == RegionType::Ram) bytes_[address] = value;
}

void MemoryBus::load(std::uint16_t address, const std::uint8_t* data, std::size_t size) {
    if (data == nullptr || size == 0) return;
    const std::size_t start = address;
    const std::size_t count = std::min(size, bytes_.size() - start);
    for (std::size_t i = 0; i < count; ++i) {
        // Loading represents a host-side ROM/RAM image operation, so it can
        // initialize ROM bytes without making them writable by the emulated CPU.
        if (regions_[start + i] != RegionType::Unmapped) bytes_[start + i] = data[i];
    }
}

void MemoryBus::mapRegion(std::uint16_t start, std::size_t size, RegionType type,
                          std::uint8_t fill) {
    const std::size_t begin = start;
    const std::size_t count = std::min(size, bytes_.size() - begin);
    for (std::size_t i = 0; i < count; ++i) {
        regions_[begin + i] = type;
        bytes_[begin + i] = fill;
    }
}

MemoryBus::RegionType MemoryBus::regionType(std::uint16_t address) const {
    return regions_[address];
}
} // namespace pc6001
