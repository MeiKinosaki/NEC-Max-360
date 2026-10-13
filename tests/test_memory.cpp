#include "machine/memory.h"

#include <cassert>
#include <cstdint>

int main() {
    pc6001::MemoryBus memory;
    assert(memory.read(0x1234) == 0);
    memory.write(0x1234, 0xA5);
    assert(memory.read(0x1234) == 0xA5);
    memory.reset();
    assert(memory.read(0x1234) == 0);
    memory.write(0xFFFF, 0x5A);
    assert(memory.read(0xFFFF) == 0x5A);

    using Region = pc6001::MemoryBus::RegionType;
    memory.mapRegion(0x0000, 0x2000, Region::Rom, 0xFF);
    const std::uint8_t rom[] = {0x31, 0x00, 0x40};
    memory.load(0x0100, rom, sizeof(rom));
    assert(memory.regionType(0x0100) == Region::Rom);
    assert(memory.read(0x0100) == 0x31);
    memory.write(0x0100, 0x99);
    assert(memory.read(0x0100) == 0x31); // CPU writes to ROM are ignored.

    memory.mapRegion(0x4000, 0x1000, Region::Unmapped, 0x00);
    assert(memory.regionType(0x4000) == Region::Unmapped);
    assert(memory.read(0x4000) == 0xFF);
    memory.write(0x4000, 0x12);
    assert(memory.read(0x4000) == 0xFF);

    memory.mapRegion(0x2000, 0x1000, Region::Ram, 0x00);
    memory.write(0x2000, 0xA7);
    assert(memory.read(0x2000) == 0xA7);
    return 0;
}
