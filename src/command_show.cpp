#include "commands.hpp"
#include "devices/ikkegol_pedal.hpp"
#include "configuration/keyboard.hpp"
#include "configuration/text.hpp"
#include "configuration/mouse.hpp"
#include "configuration/gamepad.hpp"
#include "configuration/media.hpp"
#include "configuration/dumper.hpp"
#include "utils/command_line.hpp"
#include <iostream>

void printConfig(SharedConfiguration config);
void printKeyboardConfig(KeyboardConfiguration &config);
void printTextConfig(TextConfiguration &config);
void printMouseConfig(MouseConfiguration &config);
void printGamepadConfig(GamepadConfiguration &config);
void printMediaConfig(MediaConfiguration &config);

void printShowHelp(const std::string_view &name) {
    std::cerr
        << "Usage: " << name << " show { DEVICE | help }" << std::endl
        << std::endl
        << "  Shows the current configuration of a device" << std::endl
        << std::endl
        << "ARGUMENTS" << std::endl
        << "  DEVICE\t\tThe index of the device" << std::endl
        << std::endl;
}

int showCommand(const std::string_view &name, const std::vector<std::string_view> &args) {
    uint32_t deviceId;

    if (args.empty()) {
        printShowHelp(name);
        return 1;
    }

    if (args[0] == "help") {
        printShowHelp(name);
        return 0;
    } else {
        auto id = parseInt(args[0]);
        if (!id || *id < 1) {
            std::cerr << "Invalid device index " << args[0] << std::endl;
            return 1;
        }

        deviceId = *id;
    }

    auto device = findIkkegolDevice(deviceId);
    if (!device) {
        std::cerr << "Unable to find device " << deviceId << std::endl;
        return 1;
    }

    if (!device->isValid()) {
        std::cerr << "Unable to load device. " << device->getLastError() << std::endl;
        return 1;
    }

    if (!device->load()) {
        std::cerr << "Unable to read configuration. " << device->getLastError() << std::endl;
        return 1;
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

    return 0;
}
