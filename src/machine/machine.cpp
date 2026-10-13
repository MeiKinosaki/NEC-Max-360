#include "machine/machine.h"

namespace pc6001 {
void Machine::reset() {
    cpu_.reset();
    memory_.reset();
}
unsigned Machine::step() {
    // Current Z80 owns a private flat test RAM. This is intentionally explicit:
    // the bus is not yet connected to instruction fetches or data accesses.
    return cpu_.step();
}
} // namespace pc6001
