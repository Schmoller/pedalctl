#pragma once

#include "base.hpp"
#include <string>
#include <vector>

enum class KeyMode {
    OneShot,
    Standard
};

class KeyboardConfiguration : public BaseConfiguration {
public:
    ConfigurationType type() const override { return ConfigurationType::Keyboard; }

    KeyMode mode { KeyMode::Standard };
    std::vector<std::string> keys;
};
