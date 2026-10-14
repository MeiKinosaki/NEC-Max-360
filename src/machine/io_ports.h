#pragma once

#include <array>
#include <cstdint>

namespace pc6001 {

// Placeholder port registry. No real PC-6001 peripheral decoding yet.
class IoPorts {
public:
    IoPorts();
    void reset();
    std::uint8_t read(std::uint16_t port) const;
    void write(std::uint16_t port, std::uint8_t value);
private:
    std::array<std::uint8_t, 65536> ports_{};
};
} // namespace pc6001
