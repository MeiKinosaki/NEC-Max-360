#include "machine/machine.h"
#include "machine/scheduler.h"

namespace pc6001 {
Machine::Machine() {
    cpu_.setBus(
        [this](std::uint16_t address) { return memory_.read(address); },
        [this](std::uint16_t address, std::uint8_t value) { memory_.write(address, value); });
    cpu_.setIoBus(
        [this](std::uint16_t port) { return ioPorts_.read(port); },
        [this](std::uint16_t port, std::uint8_t value) {
            ioPorts_.write(port, value);
        });
}
void Machine::configureBasePc6001Map() {
    useBasePc6001Map_ = true;
    memory_.configureBasePc6001Map();
}
void Machine::reset() {
    memory_.reset();
    if (useBasePc6001Map_) memory_.configureBasePc6001Map();
    ioPorts_.reset();
    cpu_.reset();
    scheduler_.reset();
}
unsigned Machine::step() {
    const unsigned cycles = cpu_.step();
    scheduler_.advance(cycles);
    return cycles;
}
} // namespace pc6001
