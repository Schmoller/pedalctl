#include "ikkegol_capabilities.hpp"

#include <map>

const std::map<std::string, Capabilities> ModelCapabilities;

const char *PedalNamesLMR[3] {
    "left",
    "middle",
    "right",
};

const char *PedalNamesSingle[1] {
    "pedal"
};

const Capabilities FS2020U1IR {
    .pedals = 3,
    .pedalNames = PedalNamesLMR
};

const Capabilities FS2017U1IR {
    .pedals = 1,
    .firstPedalIndex = 1,
    .pedalNames = PedalNamesSingle
};

std::optional<Capabilities> getModelCapabilities(const std::string_view &model) {
    if (model == "FS2020U1IR") {
        return FS2020U1IR;
    }
    if (model == "FS2017U1IR") {
        return FS2017U1IR;
    }

    return {};
}