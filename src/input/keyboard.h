#pragma once
#include <array>
#include <cstdint>
namespace pc6001 {
class Keyboard {
public:
    void reset();
    void setKey(std::uint8_t key, bool pressed);
    bool isPressed(std::uint8_t key) const;
private:
    std::array<bool, 256> pressed_{};
};
}
