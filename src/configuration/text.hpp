#pragma once

#include "base.hpp"
#include <string>

class TextConfiguration : public BaseConfiguration {
public:
    ConfigurationType type() const override { return ConfigurationType::Text; }

    std::string text;
};
