#pragma once

#include <vector>
#include <string>

int listCommand(const std::string_view &name, const std::vector<std::string_view> &args);
int showCommand(const std::string_view &name, const std::vector<std::string_view> &args);
int setCommand(const std::string_view &name, const std::vector<std::string_view> &args);