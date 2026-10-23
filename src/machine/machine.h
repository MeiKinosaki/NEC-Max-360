#pragma once

#include "cpu/z80.h"
#include "machine/memory.h"
#include "machine/io_ports.h"
#include "machine/scheduler.h"
#include "input/keyboard.h"
#include "video/framebuffer.h"
#include "audio/audio_buffer.h"

#include <cstdint>

namespace pc6001 {

// Machine shell with CPU memory accesses routed through the shared bus.
class Machine {
public:
    Machine();
    void reset();
    void configureBasePc6001Map();
    unsigned step();
    Z80& cpu() { return cpu_; }
    const Z80& cpu() const { return cpu_; }
    MemoryBus& memory() { return memory_; }
    const MemoryBus& memory() const { return memory_; }
    IoPorts& ioPorts() { return ioPorts_; }
    const IoPorts& ioPorts() const { return ioPorts_; }
    Keyboard& keyboard() { return keyboard_; }
    const Keyboard& keyboard() const { return keyboard_; }
    VideoFrame& video() { return video_; }
    const VideoFrame& video() const { return video_; }
    AudioBuffer& audio() { return audio_; }
    const AudioBuffer& audio() const { return audio_; }
    std::uint64_t totalCycles() const { return scheduler_.totalCycles(); }
private:
    Z80 cpu_{};
    MemoryBus memory_{};
    IoPorts ioPorts_{};
    Scheduler scheduler_{};
    Keyboard keyboard_{};
    VideoFrame video_{};
    AudioBuffer audio_{};
    bool useBasePc6001Map_ = false;
};
} // namespace pc6001
