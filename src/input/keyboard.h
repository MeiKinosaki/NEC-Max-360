#pragma once
#include <array>
#include <cstdint>
namespace pc6001 {
class Keyboard {
public:
    void reset();
    void setKey(std::uint8_t key, bool pressed);
    bool isPressed(std::uint8_t key) const;
    // Generic 8x8 active-low matrix scaffold; key-to-row mapping is model-configured later.
    void setMatrixKey(std::uint8_t row, std::uint8_t column, bool pressed);
    std::uint8_t readMatrixRow(std::uint8_t row) const;
private:
    std::array<bool, 256> pressed_{};
    std::array<std::array<bool, 8>, 8> matrix_{};
};
}
