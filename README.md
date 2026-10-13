# NEC-PC-6001

Early-stage emulator scaffold targeting eventual Xbox 360 homebrew support.

## Current status
- CMake C++17 host build and GitHub Actions CI.
- Partial Z80 interpreter for a subset of base opcodes.
- Flat 64 KiB memory bus scaffold and machine shell.
- Generic I/O port registry, framebuffer storage, audio sample buffer and keyboard-state scaffold.
- Smoke tests for CPU, memory and scaffold devices.

## Host build
Requires CMake 3.16+ and a C++17 compiler.
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Limitations
This is not yet a working PC-6001 emulator. The CPU opcode set and flags are incomplete and not cycle-accurate. Unsupported instructions are placeholder NOPs. The machine shell does not yet connect the memory bus to CPU fetches/data accesses. I/O ports are generic placeholders, not PC-6001 hardware decoding. The framebuffer does not emulate MC6847; audio does not emulate a sound chip; keyboard state is not mapped to the original key matrix. ROM/tape loading, interrupts, accurate timing and Xbox 360 APIs are not implemented. Host CI does not produce `default.xex`.

## Roadmap
1. Validate CPU opcode behavior against a trusted Z80 test suite.
2. Introduce a CPU bus abstraction and wire machine memory/peripherals.
3. Implement PC-6001 ROM/RAM mapping, I/O, interrupt/timer behavior.
4. Implement MC6847 video and sound hardware behavior.
5. Add keyboard matrix and tape/program loading.
6. Configure Xbox 360 toolchain and produce/test `default.xex`.

See [LICENSE](LICENSE); preserve third-party notices and comply with upstream licenses for future ports.
