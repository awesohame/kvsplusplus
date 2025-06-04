#include "kvstore/utils/Helpers.hpp"

namespace kvspp {
namespace utils {

// ANSI color code definitions
const char* ColorOutput::COLOR_GREEN = "\033[32m";
const char* ColorOutput::COLOR_RED = "\033[31m";
const char* ColorOutput::COLOR_RESET = "\033[0m";

std::string ColorOutput::passMsg(const std::string& message) {
    return std::string(COLOR_GREEN) + "[PASS]" + COLOR_RESET + " " + message;
}

std::string ColorOutput::failMsg(const std::string& message) {
    return std::string(COLOR_RED) + "[FAIL]" + COLOR_RESET + " " + message;
}

} // namespace utils
} // namespace kvspp