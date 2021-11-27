#pragma once

#include "base.hpp"
#include <string>
#include <vector>

enum class GamepadButton {
    Left,
    Right,
    Up,
    Down,
    Button1,
    Button2,
    Button3,
    Button4,
    Button5,
    Button6,
    Button7,
    Button8,
};

class GamepadConfiguration : public BaseConfiguration {
public:
    ConfigurationType type() const override { return ConfigurationType::Gamepad; }

    GamepadButton button;
};
