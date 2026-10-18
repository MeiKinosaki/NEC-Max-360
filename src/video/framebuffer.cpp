#include "video/framebuffer.h"

#include <algorithm>
#include <cstddef>

namespace pc6001 {
void VideoFrame::clear(std::uint32_t argb) {
    std::fill(pixels_.begin(), pixels_.end(), argb);
}
bool VideoFrame::setPixel(unsigned x, unsigned y, std::uint32_t argb) {
    if (x >= kWidth || y >= kHeight) return false;
    pixels_[static_cast<std::size_t>(y) * kWidth + x] = argb;
    return true;
}
std::uint32_t VideoFrame::pixel(unsigned x, unsigned y) const {
    if (x >= kWidth || y >= kHeight) return 0;
    return pixels_[static_cast<std::size_t>(y) * kWidth + x];
}
} // namespace pc6001
