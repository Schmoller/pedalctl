#pragma once

#include <optional>
#include <string>

std::optional<int> parseInt(const std::string &, int radix = 10);

inline std::optional<int> parseInt(const std::string_view &input, int radix = 10) {
    return parseInt(std::string(input), radix);
}