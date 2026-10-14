#include "machine/io_ports.h"

namespace pc6001 {
IoPorts::IoPorts() { reset(); }
void IoPorts::reset() { ports_.fill(0xFF); }
std::uint8_t IoPorts::read(std::uint16_t port) const { return ports_[port]; }
void IoPorts::write(std::uint16_t port, std::uint8_t value) { ports_[port] = value; }
} // namespace pc6001
