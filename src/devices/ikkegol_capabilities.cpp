#include "ikkegol_capabilities.hpp"

#include <map>

const std::map<std::string, Capabilities> ModelCapabilities;

const Capabilities FS2020U1IR {
    .pedals = 3
};

std::optional<Capabilities> getModelCapabilities(const std::string_view &model) {
    if (model == "FS2020U1IR") {
        return FS2020U1IR;
    }

    return {};
}