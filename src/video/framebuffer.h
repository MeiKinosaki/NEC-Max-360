#pragma once

#include <cstdint>
#include <vector>

namespace pc6001 {

// Framebuffer plumbing only. MC6847 timing, modes and attributes are not implemented.
class VideoFrame {
public:
    static constexpr unsigned kWidth = 320;
    static constexpr unsigned kHeight = 240;
    void clear(std::uint32_t argb = 0xFF000000u);
    std::uint32_t* pixels() { return pixels_.data(); }
    const std::uint32_t* pixels() const { return pixels_.data(); }
    unsigned width() const { return kWidth; }
    unsigned height() const { return kHeight; }
private:
    std::vector<std::uint32_t> pixels_ = std::vector<std::uint32_t>(kWidth * kHeight, 0xFF000000u);
};
} // namespace pc6001
