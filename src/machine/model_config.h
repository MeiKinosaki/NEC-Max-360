#pragma once
#include <cstdint>
namespace pc6001 {
enum class Model : std::uint8_t { PC6001, PC6001A, PC6001MK2, PC6001MK2SR };
struct ModelConfig {
    Model model;
    const char* displayName;
    std::uint32_t cpuClockHz;
};
// Clock values are provisional placeholders pending verification against model documentation.
constexpr ModelConfig configFor(Model model) {
    switch (model) {
    case Model::PC6001A: return {model, "PC-6001A", 3579545};
    case Model::PC6001MK2: return {model, "PC-6001mkII", 3579545};
    case Model::PC6001MK2SR: return {model, "PC-6001mkIISR", 3579545};
    default: return {Model::PC6001, "PC-6001", 3579545};
    }
}
}
