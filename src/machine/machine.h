#pragma once

#include "cpu/z80.h"
#include "machine/memory.h"

#include <cstdint>

namespace pc6001 {

// Initial machine shell. CPU currently has an internal test memory; bus/CPU
// integration is the next architectural step, so this class is scaffolding.
class Machine {
public:
    void reset();
    unsigned step();
    Z80& cpu() { return cpu_; }
    const Z80& cpu() const { return cpu_; }
    MemoryBus& memory() { return memory_; }
    const MemoryBus& memory() const { return memory_; }
private:
    Z80 cpu_{};
    MemoryBus memory_{};
};
} // namespace pc6001
