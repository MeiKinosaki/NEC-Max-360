cmake_minimum_required(VERSION 3.16)
project(NEC_PC_6001 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(pc6001_core
    src/cpu/z80.cpp
    src/machine/memory.cpp
    src/machine/machine.cpp
    src/machine/scheduler.cpp
    src/machine/io_ports.cpp
    src/video/framebuffer.cpp
    src/audio/audio_buffer.cpp
    src/input/keyboard.cpp
)
target_include_directories(pc6001_core PUBLIC src)
add_executable(pc6001 src/main.cpp)
target_link_libraries(pc6001 PRIVATE pc6001_core)

include(CTest)
if(BUILD_TESTING)
    add_executable(pc6001_tests tests/test_z80.cpp)
    target_link_libraries(pc6001_tests PRIVATE pc6001_core)
    add_test(NAME pc6001_tests COMMAND pc6001_tests)
    add_executable(pc6001_memory_tests tests/test_memory.cpp)
    target_link_libraries(pc6001_memory_tests PRIVATE pc6001_core)
    add_test(NAME pc6001_memory_tests COMMAND pc6001_memory_tests)
    add_executable(pc6001_devices_tests tests/test_devices.cpp)
    target_link_libraries(pc6001_devices_tests PRIVATE pc6001_core)
    add_test(NAME pc6001_devices_tests COMMAND pc6001_devices_tests)
endif()
