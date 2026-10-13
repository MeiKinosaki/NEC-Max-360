#include "input/keyboard.h"
namespace pc6001 {
void Keyboard::reset() { pressed_.fill(false); }
void Keyboard::setKey(std::uint8_t key, bool pressed) { pressed_[key] = pressed; }
bool Keyboard::isPressed(std::uint8_t key) const { return pressed_[key]; }
}
