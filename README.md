# NEC Max 360

Repository: https://github.com/MeiKinosaki/NEC-Max-360

**NEC Max 360** is an early-stage NEC PC-6001 emulator project targeting eventual Xbox 360 homebrew support.

## Current status
- CMake C++17 host build and GitHub Actions CI.
- Partial Z80 interpreter for a subset of base opcodes, now including `DAA`, ED-prefixed 16-bit `ADC/SBC HL,rr`, block transfers `LDI/LDD/LDIR/LDDR`, `NEG` and `RETN/RETI` aliases.
- CPU instruction and data accesses routed through a shared 64 KiB bus with configurable RAM, ROM and unmapped regions; an explicit baseline PC-6001 map helper is available (0000-3FFF ROM placeholder, 4000-BFFF unmapped expansion window, C000-FFFF RAM), with reset preserving loaded ROM and clearing mapped RAM; this must be verified against the exact PC-6001 model and firmware before claiming hardware accuracy.
- Machine shell with instruction-cycle accounting and owned keyboard, framebuffer and audio-buffer subsystems; machine reset clears volatile peripheral state.
- Z80 immediate `IN A,(n)` / `OUT (n),A` plus ED-prefixed `IN r,(C)` / `OUT (C),r`, connected to a generic 16-bit port registry; framebuffer storage with bounds-checked pixel access, audio sample buffer and generic active-low 8×8 keyboard-matrix scaffold (original PC-6001 key mapping remains pending).
- Host regression tests for CPU control flow, stack operations, 16-bit transfers, ED-prefixed arithmetic and block transfers, `NEG`/`RETN`, INC/DEC flags, parity, undocumented arithmetic/logic flag bits and `DAA` decimal adjustment; machine-level baseline memory-map setup and CPU execution across ROM/RAM/unmapped regions; memory bus, scheduler, keyboard matrix and framebuffer bounds; integrated peripheral reset lifecycle.
- CMake target names aligned with the `NEC Max 360` project identity.

## Host build
Requires CMake 3.16+ and a C++17 compiler.
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Limitations
This is not yet a complete or verified PC-6001 emulator. The CPU opcode set remains incomplete and is not cycle-accurate; selected undocumented flag bits, parity behavior, decimal-adjust cases, ED arithmetic and block-transfer behavior have regression coverage, but full reference-suite validation is still required. `RETN`/`RETI` currently return through the stack but do not restore interrupt flip-flops because interrupt state is not implemented. Unsupported instructions are placeholder operations. The bus supports RAM, ROM and unmapped regions and includes a baseline map helper (0000-3FFF ROM placeholder, 4000-BFFF unmapped, C000-FFFF RAM); machine reset now preserves ROM bytes and clears mapped RAM for the selected baseline map; the exact address map can vary by model/configuration and still needs authoritative validation, real firmware images and hardware-specific overlays are not yet integrated. I/O instructions currently route through a generic 16-bit port registry, using the Z80 port-address conventions (A:n for immediate I/O and BC for ED-prefixed I/O); PC-6001 hardware decoding, accurate port timing and peripheral side effects remain unimplemented. The framebuffer does not emulate MC6847 timing, attributes or graphics modes; audio does not emulate the machine's sound hardware; keyboard matrix rows are generic and not mapped to the original PC-6001 key matrix. ROM/tape loading, interrupts, accurate device timing and Xbox 360 APIs are not implemented. Host CI does not produce `default.xex`. The machine now owns and resets the keyboard, video framebuffer and audio buffer, but those peripherals are still scaffolds and are not yet connected to PC-6001 hardware register semantics.

## Roadmap
1. Validate CPU opcode behavior against a trusted Z80 test suite.
2. Expand Z80 instructions, flags and CPU regression tests.
3. Validate the baseline PC-6001 memory map against model-specific documentation, integrate legitimate firmware images, decode real I/O ports and add interrupt/timer behavior.
4. Implement MC6847 video and sound hardware behavior.
5. Add keyboard matrix and tape/program loading.
6. Configure Xbox 360 toolchain and produce/test `default.xex`.

See [LICENSE](LICENSE); preserve third-party notices and comply with upstream licenses for future ports.


Current implementation note: ED-prefixed I/O register operations (`IN r,(C)` / `OUT (C),r`) have initial support; their edge cases and undocumented flags still require validation against a Z80 reference test suite.
