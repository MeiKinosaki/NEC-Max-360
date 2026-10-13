# NEC Max 360

Repository: https://github.com/MeiKinosaki/NEC-Max-360

**NEC Max 360** is an early-stage NEC PC-6001 emulator project targeting eventual Xbox 360 homebrew support.

## Current status
- CMake C++17 host build and GitHub Actions CI.
- Partial Z80 interpreter for a subset of base opcodes.
- CPU instruction and data accesses routed through a shared flat 64 KiB memory bus.
- Machine shell with instruction-cycle accounting.
- Generic I/O port registry, framebuffer storage, audio sample buffer and keyboard-state scaffold.
- Host regression tests for CPU load/store/arithmetic/call/return and INC/DEC flags, memory bus, scheduler and scaffold devices.
- CMake target names aligned with the `NEC Max 360` project identity.

## Host build
Requires CMake 3.16+ and a C++17 compiler.
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Limitations
This is not yet a complete or verified PC-6001 emulator. The CPU opcode set and flags are incomplete and not cycle-accurate. Unsupported instructions are placeholder operations. The memory bus is still a flat 64 KiB model, not the original PC-6001 memory map. I/O ports are generic placeholders, not PC-6001 hardware decoding. The framebuffer does not emulate MC6847; audio does not emulate the machine's sound hardware; keyboard state is not mapped to the original key matrix. ROM/tape loading, interrupts, accurate device timing and Xbox 360 APIs are not implemented. Host CI does not produce `default.xex`.

## Roadmap
1. Validate CPU opcode behavior against a trusted Z80 test suite.
2. Expand Z80 instructions, flags and CPU regression tests.
3. Implement PC-6001 ROM/RAM mapping, I/O, interrupt/timer behavior.
4. Implement MC6847 video and sound hardware behavior.
5. Add keyboard matrix and tape/program loading.
6. Configure Xbox 360 toolchain and produce/test `default.xex`.

See [LICENSE](LICENSE); preserve third-party notices and comply with upstream licenses for future ports.
