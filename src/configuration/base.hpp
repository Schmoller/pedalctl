#pragma once

#include <memory>

enum class ConfigurationType {
    Keyboard,
    Mouse,
    Text,
    Media,
    Gamepad,
};

enum class Trigger {
    OnPress,
    OnRelease
};

class BaseConfiguration {
public:
    virtual ConfigurationType type() const = 0;

    Trigger trigger { Trigger::OnPress };

private:
};

typedef std::shared_ptr<BaseConfiguration> SharedConfiguration;