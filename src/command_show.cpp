#include "commands.hpp"
#include "devices/ikkegol_pedal.hpp"
#include "configuration/keyboard.hpp"
#include "configuration/text.hpp"
#include <iostream>

void printConfig(SharedConfiguration config);
void printKeyboardConfig(KeyboardConfiguration &config);
void printTextConfig(TextConfiguration &config);

void showCommand(args::Subparser &parser) {
    args::Positional<std::string> deviceId(parser, "device", "Device ID of the pedal", args::Options::Required);

    parser.Parse();

    auto device = findIkkegolDevice(deviceId.Get());
    if (!device) {
        std::cerr << "Unable to find device " << deviceId.Get() << std::endl;
        exit(1);
    }

    if (!device->load()) {
        std::cerr << "Unable to read configuration" << std::endl;
        exit(1);
    }

    std::cout << "Device information:" << std::endl;
    std::cout << std::endl;
    std::cout << "Model: " << device->getModel() << std::endl;
    std::cout << "Pedals: " << device->getPedalCount() << std::endl;

    std::cout << std::endl;
    for (auto pedal = 0; pedal < device->getPedalCount(); ++pedal) {
        std::cout << "Pedal " << (pedal + 1) << ":" << std::endl;
        printConfig(device->getConfiguration(pedal));
    }
}

void printConfig(SharedConfiguration config) {
    if (!config) {
        std::cout << "  No configuration" << std::endl;
        return;
    }

    std::cout << "  Trigger: ";
    if (config->trigger == Trigger::OnPress) {
        std::cout << "On press" << std::endl;
    } else {
        std::cout << "On release" << std::endl;
    }

    switch (config->type()) {
        case ConfigurationType::Keyboard: {
            printKeyboardConfig(static_cast<KeyboardConfiguration &>(*config));
            break;
        }
        case ConfigurationType::Mouse: {
            std::cout << "  mouse Not implemented" << std::endl;
            break;
        }
        case ConfigurationType::Text: {
            printTextConfig(static_cast<TextConfiguration &>(*config));
            break;
        }
        case ConfigurationType::Media: {
            std::cout << "  media Not implemented" << std::endl;
            break;
        }
        case ConfigurationType::Game: {
            std::cout << "  game Not implemented" << std::endl;
            break;
        }
        default: {
            std::cout << "Not implemented" << std::endl;
            break;
        }
    };
}

void printKeyboardConfig(KeyboardConfiguration &config) {
    std::cout << "  Mode: ";
    if (config.mode == KeyMode::Standard) {
        std::cout << "Standard" << std::endl;
    } else {
        std::cout << "One shot" << std::endl;
    }

    std::cout << "  Key: ";
    bool first = true;
    for (auto &key: config.keys) {
        if (!first) {
            std::cout << " + ";
        }
        first = false;
        std::cout << key;
    }
    std::cout << std::endl;
}

void printTextConfig(TextConfiguration &config) {
    std::cout << " Text: " << config.text << std::endl;
}