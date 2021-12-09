#include "commands.hpp"
#include "command_set.hpp"
#include "devices/ikkegol_pedal.hpp"
#include "configuration/dumper.hpp"
#include <iostream>

void printSetHelp(const std::string_view &name) {
    std::cerr
        << "Usage: " << name << " set DEVICE PEDAL TYPE { ARGS | help }" << std::endl
        << std::endl
        << "  Sets the configuration for a device." << std::endl
        << std::endl
        << "ARGUMENTS" << std::endl
        << "  DEVICE\t\tThe device index to configure" << std::endl
        << "  PEDAL\t\t\tThe pedal name or index" << std::endl
        << std::endl
        << "TYPE" << std::endl
        << "  keyboard\t\tActs as a keyboard key or key combination" << std::endl
        << "  mouse\t\t\tActs as a mouse button or mouse movement" << std::endl
        << "  text\t\t\tPrints some text" << std::endl
        << "  gamepad\t\tActs as game controller" << std::endl
        << "  media\t\t\tActs as a media key" << std::endl
        << std::endl;
}

std::optional<int> parsePedal(const std::string &rawPedal, const SharedIkkegolPedal &device);


int setCommand(const std::string_view &name, const std::vector<std::string_view> &args) {
    if (!args.empty() && args[0] == "help") {
        printSetHelp(name);
        return 0;
    }

    if (args.size() < 3) {
        std::cerr << "Not enough arguments" << std::endl;
        printSetHelp(name);
        return 1;
    }

    uint32_t deviceId;
    uint32_t pedal;

    // Device
    try {
        deviceId = stoi(std::string(args[0]));
        if (deviceId < 1) {
            std::cerr << "Invalid device index " << args[0] << std::endl;
            return 1;
        }

    } catch (std::invalid_argument &error) {
        std::cerr << "Invalid device index " << args[0] << std::endl;
        return 1;
    }

    auto device = findIkkegolDevice(deviceId);
    if (!device) {
        std::cerr << "Unable to find device " << deviceId << std::endl;
        return 1;
    }

    // Pedal
    auto optionalPedal = parsePedal(std::string(args[1]), device);
    if (!optionalPedal) {
        std::cerr << "Invalid pedal name or index '" << args[1] << "'" << std::endl;
        std::cerr << "Possible values:" << std::endl;
        std::cerr << " ";
        for (auto pedalIndex = 0; pedalIndex < device->getPedalCount(); ++pedalIndex) {
            if (pedalIndex != 0) {
                std::cerr << ", ";
            }

            auto pedalName = device->getPedalName(pedalIndex);
            std::cerr << (pedalIndex + 1);
            if (!pedalName.empty()) {
                std::cerr << ", " << pedalName;
            }
        }
        std::cerr << std::endl;
        return 1;
    }

    pedal = *optionalPedal;

    std::optional<SharedConfiguration> config;
    auto &commandName = args[2];
    std::vector<std::string_view> commandArgs { args.begin() + 3, args.end() };

    if (commandName == "keyboard") {
        config = parseSetKeyboardOptions(name, commandArgs);
    } else if (commandName == "mouse") {
        config = parseSetMouseOptions(name, commandArgs);
    } else if (commandName == "text") {
        config = parseSetTextOptions(name, commandArgs);
    } else if (commandName == "media") {
        config = parseSetMediaOptions(name, commandArgs);
    } else if (commandName == "game") {
        config = parseSetGameOptions(name, commandArgs);
    } else {
        std::cerr << "Unknown configuration type " << commandName << std::endl;
        printSetHelp(name);
        return 1;
    }

    if (!config) {
        return 1;
    }

    device->setConfiguration(pedal, *config);

    if (!device->save()) {
        std::cerr << "Unable to write configuration" << std::endl;
        return 1;
    }

    std::cout << "Updated configuration" << std::endl;
    printConfig(*config);
    return 0;
}

std::optional<int> parsePedal(const std::string &rawPedal, const SharedIkkegolPedal &device) {
    uint32_t pedalIndex;

    try {
        pedalIndex = stoi(rawPedal);
        if (pedalIndex < 1 || pedalIndex > device->getPedalCount()) {
            std::cerr << "Invalid pedal index. Expected an index between 1 and " << device->getPedalCount()
                << std::endl;
            exit(1);
        }
        --pedalIndex;
    } catch (std::invalid_argument &error) {
        auto found = false;
        for (pedalIndex = 0; pedalIndex < device->getPedalCount(); ++pedalIndex) {
            auto name = device->getPedalName(pedalIndex);
            if (!name.empty() && name == rawPedal) {
                found = true;
                break;
            }
        }
        if (!found) {
            return {};
        }
    }

    return pedalIndex;
}