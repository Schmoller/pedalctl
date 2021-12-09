#pragma once

#include "configuration/base.hpp"
#include <vector>
#include <optional>
#include <string>

std::optional<SharedConfiguration> parseSetKeyboardOptions(
    const std::string_view &name, const std::vector<std::string_view> &args
);
std::optional<SharedConfiguration> parseSetMouseOptions(
    const std::string_view &name, const std::vector<std::string_view> &args
);
std::optional<SharedConfiguration> parseSetTextOptions(
    const std::string_view &name, const std::vector<std::string_view> &args
);
std::optional<SharedConfiguration> parseSetMediaOptions(
    const std::string_view &name, const std::vector<std::string_view> &args
);
std::optional<SharedConfiguration> parseSetGameOptions(
    const std::string_view &name, const std::vector<std::string_view> &args
);