#include "ikkegol_capabilities.hpp"

#include <map>

const std::map<std::string, Capabilities> ModelCapabilities;

const char *PedalNamesLMR[3] {
    "left",
    "middle",
    "right",
};

const Capabilities FS2020U1IR {
    .pedals = 3,
    .pedalNames = PedalNamesLMR
};

std::optional<Capabilities> getModelCapabilities(const std::string_view &model) {
    if (model == "FS2020U1IR") {
        return FS2020U1IR;
    }

    return {};
}