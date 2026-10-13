cmake_minimum_required(VERSION 3.16)
project(NEC_Max_360 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(necmax360_core
    src/cpu/z80.cpp
    src/machine/memory.cpp
    src/machine/machine.cpp
    src/machine/scheduler.cpp
    src/machine/io_ports.cpp
    src/video/framebuffer.cpp
    src/audio/audio_buffer.cpp
    src/input/keyboard.cpp
)
target_include_directories(necmax360_core PUBLIC src)

add_executable(necmax360 src/main.cpp)
target_link_libraries(necmax360 PRIVATE necmax360_core)

include(CTest)
if(BUILD_TESTING)
    add_executable(necmax360_cpu_tests tests/test_z80.cpp)
    target_link_libraries(necmax360_cpu_tests PRIVATE necmax360_core)
    add_test(NAME necmax360_cpu_tests COMMAND necmax360_cpu_tests)

    add_executable(necmax360_memory_tests tests/test_memory.cpp)
    target_link_libraries(necmax360_memory_tests PRIVATE necmax360_core)
    add_test(NAME necmax360_memory_tests COMMAND necmax360_memory_tests)

    add_executable(necmax360_devices_tests tests/test_devices.cpp)
    target_link_libraries(necmax360_devices_tests PRIVATE necmax360_core)
    add_test(NAME necmax360_devices_tests COMMAND necmax360_devices_tests)

    add_executable(necmax360_scheduler_tests tests/test_scheduler.cpp)
    target_link_libraries(necmax360_scheduler_tests PRIVATE necmax360_core)
    add_test(NAME necmax360_scheduler_tests COMMAND necmax360_scheduler_tests)
endif()
