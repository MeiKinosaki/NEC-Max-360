#pragma once
#include <cstdint>
namespace pc6001 {
// Deterministic cycle accumulator scaffold; not yet connected to video/audio/interrupt timing.
class Scheduler {
public:
    void reset() { cycles_ = 0; }
    void advance(std::uint32_t cycles) { cycles_ += cycles; }
    std::uint64_t totalCycles() const { return cycles_; }
private:
    std::uint64_t cycles_ = 0;
};
}
