#include "command_set.hpp"
#include "configuration/keyboard.hpp"
#include "configuration/keys.hpp"
#include "utils/string_utils.hpp"
#include "configuration/gamepad.hpp"
#include <iostream>

void printSetGameHelp(const std::string_view &name) {
    std::cerr
        << "Usage: " << name << " set DEVICE PEDAL gamepad [OPTIONS] BUTTON" << std::endl
        << std::endl
        << "  Sets a pedal to act as a game controller / joystick button" << std::endl
        << std::endl
        << "ARGUMENTS" << std::endl
        << "  DEVICE\t\tThe device index to configure" << std::endl
        << "  PEDAL\t\t\tThe pedal name or index" << std::endl
        << "  BUTTON\t\t\tThe button name. See below"
        << std::endl
        << "OPTIONS" << std::endl
        << "  -i, --invert\t\tInverts pedal activation. The button will be pressed when" << std::endl
        << "  \t\t\tthe pedal is released." << std::endl
        << "BUTTON" << std::endl
        << "  Available gamepad keys:" << std::endl
        << "    left, right, up, down, button1, button2, button3, button4, button5" << std::endl
        << "    button6, button7, button8" << std::endl
        << std::endl;
}


std::optional<SharedConfiguration> parseSetGameOptions(
    const std::string_view &name, const std::vector<std::string_view> &args
) {
    if (args.empty()) {
        printSetGameHelp(name);
        return {};
    }

    bool invert = false;

    size_t nextArgIndex;
    // Options first
    for (nextArgIndex = 0; nextArgIndex < args.size(); ++nextArgIndex) {
        auto &arg = args[nextArgIndex];

        if (arg.empty()) {
            continue;
        }

        // No more options after this
        if (arg == "--" || arg[0] != '-') {
            break;
        }

        if (arg == "-i" || arg == "--invert") {
            if (invert) {
                std::cerr << "--invert already set. This flag can only be specified once" << std::endl;
                printSetGameHelp(name);
                return {};
            }
            invert = true;
        } else {
            std::cerr << "Unknown option " << arg << std::endl;
            printSetGameHelp(name);
            return {};
        }
    }

    if (nextArgIndex >= args.size()) {
        std::cerr << "Missing button" << std::endl;
        printSetGameHelp(name);
        return {};
    }

    auto config = std::make_shared<GamepadConfiguration>();
    auto &button = args[nextArgIndex];

    if (invert) {
        config->trigger = Trigger::OnRelease;
    } else {
        config->trigger = Trigger::OnPress;
    }

    if (button == "left") {
        config->button = GamepadButton::Left;
    } else if (button == "right") {
        config->button = GamepadButton::Right;
    } else if (button == "up") {
        config->button = GamepadButton::Up;
    } else if (button == "down") {
        config->button = GamepadButton::Down;
    } else if (button == "button1") {
        config->button = GamepadButton::Button1;
    } else if (button == "button2") {
        config->button = GamepadButton::Button2;
    } else if (button == "button3") {
        config->button = GamepadButton::Button3;
    } else if (button == "button4") {
        config->button = GamepadButton::Button4;
    } else if (button == "button5") {
        config->button = GamepadButton::Button5;
    } else if (button == "button6") {
        config->button = GamepadButton::Button6;
    } else if (button == "button7") {
        config->button = GamepadButton::Button7;
    } else if (button == "button8") {
        config->button = GamepadButton::Button8;
    } else {
        std::cerr << "Unknown button " << button << std::endl;
        printSetGameHelp(name);
        return {};
    }

    return config;
}
