#include "input/keyboard.h"
#include <cstddef>
namespace pc6001 {
void Keyboard::reset() {
    pressed_.fill(false);
    for (auto& row : matrix_) row.fill(false);
}
void Keyboard::setKey(std::uint8_t key, bool pressed) { pressed_[key] = pressed; }
bool Keyboard::isPressed(std::uint8_t key) const { return pressed_[key]; }
void Keyboard::setMatrixKey(std::uint8_t row, std::uint8_t column, bool pressed) {
    if (row >= matrix_.size() || column >= matrix_[0].size()) return;
    matrix_[row][column] = pressed;
}
std::uint8_t Keyboard::readMatrixRow(std::uint8_t row) const {
    if (row >= matrix_.size()) return 0xFF;
    std::uint8_t value = 0xFF;
    for (std::size_t column = 0; column < matrix_[row].size(); ++column) {
        if (matrix_[row][column]) value = static_cast<std::uint8_t>(value & ~(1u << column));
    }
    return value;
}
}
