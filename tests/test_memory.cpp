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
    return 0;
}
