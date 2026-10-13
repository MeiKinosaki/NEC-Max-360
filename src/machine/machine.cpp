#include "machine/machine.h"
#include "machine/scheduler.h"

namespace pc6001 {
Machine::Machine() {
    cpu_.setBus(
        [this](std::uint16_t address) { return memory_.read(address); },
        [this](std::uint16_t address, std::uint8_t value) { memory_.write(address, value); });
}
void Machine::reset() {
    memory_.reset();
    cpu_.reset();
    scheduler_.reset();
}
unsigned Machine::step() {
    const unsigned cycles = cpu_.step();
    scheduler_.advance(cycles);
    return cycles;
}
} // namespace pc6001
