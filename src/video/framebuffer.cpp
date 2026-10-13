#include "video/framebuffer.h"

#include <algorithm>

namespace pc6001 {
void VideoFrame::clear(std::uint32_t argb) {
    std::fill(pixels_.begin(), pixels_.end(), argb);
}
} // namespace pc6001
