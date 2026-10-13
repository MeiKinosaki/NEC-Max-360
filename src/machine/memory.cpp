#include "machine/memory.h"

#include <algorithm>

namespace pc6001 {
MemoryBus::MemoryBus() { reset(); }
void MemoryBus::reset() { bytes_.fill(0); }
std::uint8_t MemoryBus::read(std::uint16_t address) const { return bytes_[address]; }
void MemoryBus::write(std::uint16_t address, std::uint8_t value) { bytes_[address] = value; }
void MemoryBus::load(std::uint16_t address, const std::uint8_t* data, std::size_t size) {
    if (data == nullptr || size == 0) return;
    const std::size_t start = address;
    const std::size_t count = std::min(size, bytes_.size() - start);
    std::copy_n(data, count, bytes_.begin() + start);
}
} // namespace pc6001
