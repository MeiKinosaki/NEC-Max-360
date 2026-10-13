# NEC-PC-6001

Early-stage NEC PC-6001 emulator project, targeting eventual Xbox 360 homebrew support.

## Current development status

This repository is a bring-up scaffold, not yet a working PC-6001 emulator.

- Initial CMake host build and GitHub Actions test workflow.
- Early Z80 interpreter subset: basic register transfers, arithmetic/logical operations, selected loads/stores, branches, CALL/RET, stack operations and HALT.
- Basic instruction smoke tests.
- Temporary flat 64 KiB memory bus placeholder.

## Build and test on a host machine

Requirements: CMake 3.16+ and a C++17 compiler.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Roadmap

1. Expand and validate Z80 opcode coverage, flags, interrupts and timing.
2. Implement the actual PC-6001 memory/ROM map and peripheral I/O.
3. Implement MC6847-compatible video, keyboard/input, timers and audio.
4. Add model selection and program/tape loading.
5. Port rendering, audio and input to Xbox 360 homebrew APIs.
6. Configure a compatible Xbox 360 toolchain and produce/test `default.xex`.

## Important limitations

The current CPU is incomplete and is not cycle-accurate. Unsupported instructions currently act as placeholder NOPs, which can conceal incompatibilities. The memory bus is not yet wired into the CPU, and there is no video/audio/peripheral implementation. The host CI does not produce an Xbox 360 executable.

## Licensing

See [LICENSE](LICENSE). Keep upstream emulator source licenses and notices intact when evaluating code for future ports.
