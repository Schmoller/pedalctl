#pragma once

#include "base.hpp"
#include <string>
#include <vector>

enum class MultiMediaButton {
    DecreaseVolume,
    IncreaseVolume,
    Mute,
    Play,
    Forward,
    Next,
    Stop,
    OpenPlayer,
    OpenHomepage,
    StopWebPage,
    NavigateBack,
    NavigateForward,
    Refresh,
    OpenMyComputer,
    OpenMail,
    OpenCalc,
    OpenSearch,
    Shutdown,
    Sleep
};

class MediaConfiguration : public BaseConfiguration {
public:
    ConfigurationType type() const override { return ConfigurationType::Media; }

    MultiMediaButton button;
};
