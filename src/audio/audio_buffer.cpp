#include "audio/audio_buffer.h"

namespace pc6001 {
AudioBuffer::AudioBuffer(std::size_t capacity) : capacity_(capacity) {
    samples_.reserve(capacity_);
}
void AudioBuffer::clear() { samples_.clear(); }
bool AudioBuffer::push(std::int16_t sample) {
    if (samples_.size() >= capacity_) return false;
    samples_.push_back(sample);
    return true;
}
} // namespace pc6001
