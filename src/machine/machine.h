#pragma once

#include "cpu/z80.h"
#include "machine/memory.h"
#include "machine/scheduler.h"

#include <cstdint>

namespace pc6001 {

// Machine shell with CPU memory accesses routed through the shared bus.
class Machine {
public:
    Machine();
    void reset();
    unsigned step();
    Z80& cpu() { return cpu_; }
    const Z80& cpu() const { return cpu_; }
    MemoryBus& memory() { return memory_; }
    const MemoryBus& memory() const { return memory_; }
    std::uint64_t totalCycles() const { return scheduler_.totalCycles(); }
private:
    Z80 cpu_{};
    MemoryBus memory_{};
    Scheduler scheduler_{};
};
} // namespace pc6001
