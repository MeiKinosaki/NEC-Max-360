#include "machine/memory.h"

namespace pc6001 {

MemoryBus::MemoryBus() { reset(); }
void MemoryBus::reset() { bytes_.fill(0); }
std::uint8_t MemoryBus::read(std::uint16_t address) const { return bytes_[address]; }
void MemoryBus::write(std::uint16_t address, std::uint8_t value) { bytes_[address] = value; }

} // namespace pc6001
