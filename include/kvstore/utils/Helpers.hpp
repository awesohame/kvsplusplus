#pragma once

#include <string>

namespace kvspp {
namespace utils {

/**
 * @brief Utility functions for colored console output and formatting
 */
class ColorOutput {
public:
    /**
     * @brief Create a green [PASS] message with the given text
     * @param message The message to display after [PASS]
     * @return Formatted string with green [PASS] marker
     */
    static std::string passMsg(const std::string& message);

    /**
     * @brief Create a red [FAIL] message with the given text
     * @param message The message to display after [FAIL]
     * @return Formatted string with red [FAIL] marker
     */
    static std::string failMsg(const std::string& message);

private:
    // ANSI color codes
    static const char* COLOR_GREEN;
    static const char* COLOR_RED;
    static const char* COLOR_RESET;
};

} // namespace utils
} // namespace kvspp