#include "audio/audio_buffer.h"
#include "input/keyboard.h"
#include "machine/io_ports.h"
#include "machine/machine.h"
#include "video/framebuffer.h"
#include <cassert>
#include <cstdint>

int main() {
    pc6001::Machine machine;
    machine.reset();
    const std::uint8_t program[] = {
        0x3E, 0x5A,       // LD A,0x5A
        0x32, 0x00, 0x40, // LD (0x4000),A
        0x76
    };
    machine.memory().load(0, program, sizeof(program));
    assert(machine.step() == 7);
    assert(machine.cpu().regs.a == 0x5A);
    assert(machine.step() == 13);
    assert(machine.memory().read(0x4000) == 0x5A);
    assert(machine.step() == 4);
    assert(machine.cpu().regs.halted);
    assert(machine.totalCycles() == 24);

    pc6001::IoPorts ports;
    assert(ports.read(0x20) == 0xFF);
    ports.write(0x20, 0x42);
    assert(ports.read(0x20) == 0x42);
    pc6001::Keyboard keyboard;
    keyboard.reset();
    keyboard.setKey(0x41, true);
    assert(keyboard.isPressed(0x41));
    keyboard.setKey(0x41, false);
    assert(!keyboard.isPressed(0x41));
    pc6001::VideoFrame frame;
    frame.clear(0xFF123456u);
    assert(frame.width() == 320 && frame.height() == 240);
    assert(frame.pixels()[0] == 0xFF123456u);
    pc6001::AudioBuffer audio(2);
    assert(audio.push(10) && audio.push(-10));
    assert(!audio.push(30));
    audio.clear();
    assert(audio.samples().empty());
    return 0;
}
