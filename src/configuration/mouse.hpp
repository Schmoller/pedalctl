#pragma once

#include "base.hpp"
#include <string>
#include <vector>
#include <unordered_set>

enum class MouseMode {
    Buttons,
    Axis,
};

enum class MouseButton {
    Left,
    Right,
    Middle,
    Back,
    Forward,
};

class MouseConfiguration : public BaseConfiguration {
public:
    ConfigurationType type() const override { return ConfigurationType::Mouse; }

    MouseMode mode;

    std::unordered_set<MouseButton> buttons;
    int8_t relativeX { 0 };
    int8_t relativeY { 0 };
    int8_t wheelDelta { 0 };
};
