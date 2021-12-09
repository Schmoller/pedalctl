#include <stdexcept>
#include "command_line.hpp"

std::optional<int> parseInt(const std::string &input, int radix) {
    try {
        return std::stoi(input, 0, radix);
    } catch (std::invalid_argument &error) {
        return {};
    }
}
