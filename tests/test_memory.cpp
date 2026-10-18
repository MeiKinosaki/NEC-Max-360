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
    // Explicit baseline PC-6001 map: ROM, unmapped expansion window, internal RAM.
    memory.reset();
    memory.configureBasePc6001Map();
    assert(memory.regionType(0x0000) == Region::Rom);
    assert(memory.regionType(0x3FFF) == Region::Rom);
    assert(memory.read(0x0000) == 0xFF); // ROM placeholder until host loads firmware.
    memory.write(0x0000, 0x12);
    assert(memory.read(0x0000) == 0xFF); // CPU cannot write ROM.
    assert(memory.regionType(0x4000) == Region::Unmapped);
    assert(memory.regionType(0xBFFF) == Region::Unmapped);
    assert(memory.read(0x4000) == 0xFF);
    assert(memory.regionType(0xC000) == Region::Ram);
    assert(memory.regionType(0xFFFF) == Region::Ram);
    memory.write(0xC000, 0x63);
    assert(memory.read(0xC000) == 0x63);
    const std::uint8_t firmware[] = {0xC3, 0x00, 0xC0};
    memory.load(0x0000, firmware, sizeof(firmware));
    assert(memory.read(0x0000) == 0xC3);

    return 0;
}
