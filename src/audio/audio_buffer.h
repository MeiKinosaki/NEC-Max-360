#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace pc6001 {

// Audio output buffer scaffold; chip emulation and sample generation are future work.
class AudioBuffer {
public:
    explicit AudioBuffer(std::size_t capacity = 4096);
    void clear();
    bool push(std::int16_t sample);
    const std::vector<std::int16_t>& samples() const { return samples_; }
private:
    std::size_t capacity_;
    std::vector<std::int16_t> samples_;
};
} // namespace pc6001
