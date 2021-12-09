#include "command_set.hpp"
#include "configuration/mouse.hpp"
#include "utils/string_utils.hpp"
#include "utils/command_line.hpp"
#include <iostream>

void printSetMouseHelp(const std::string_view &name) {
    std::cerr
        << "Usage: " << name << " set DEVICE PEDAL mouse [OPTIONS] { BUTTONS | X Y [ WHEEL ] }" << std::endl
        << std::endl
        << "  Sets a pedal to act as a mouse." << std::endl
        << std::endl
        << "  The pedal can either be one or more mouse buttons, or a mouse movement." << std::endl
        << std::endl
        << "ARGUMENTS" << std::endl
        << "  DEVICE\t\tThe device index to configure" << std::endl
        << "  PEDAL\t\t\tThe pedal name or index" << std::endl
        << "  BUTTONS\t\tOne of more mouse buttons to press. Multiple can be combined with +" << std::endl
        << "  X\t\t\tAmount of horizontal movement to do. Must be between -100 and 100" << std::endl
        << "  Y\t\t\tAmount of vertical movement to do. Must be between -100 and 100" << std::endl
        << "  WHEEL\t\t\tNumber of notches to scroll. Must be between -100 and 100" << std::endl
        << std::endl
        << "OPTIONS" << std::endl
        << "  -i, --invert\t\tInverts pedal activation. The button or movement will trigger as " << std::endl
        << "  \t\t\tlong as the pedal is not pressed." << std::endl
        << std::endl
        << "BUTTONS" << std::endl
        << "  Available mouse buttons:" << std::endl
        << "    left, right, middle, forward, back" << std::endl
        << std::endl
        << "  Multiple mouse buttons may be combined with +, for example: left+forward" << std::endl
        << std::endl;
}


std::optional<SharedConfiguration> parseSetMouseOptions(
    const std::string_view &name, const std::vector<std::string_view> &args
) {
    if (args.empty()) {
        printSetMouseHelp(name);
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
                printSetMouseHelp(name);
                return {};
            }
            invert = true;
        } else if (std::isdigit(arg[1])) {
            // Its a negative number
            break;
        } else {
            std::cerr << "Unknown option " << arg << std::endl;
            printSetMouseHelp(name);
            return {};
        }
    }

    if (nextArgIndex >= args.size()) {
        std::cerr << "Missing mouse buttons or movement" << std::endl;
        printSetMouseHelp(name);
        return {};
    }

    auto config = std::make_shared<MouseConfiguration>();

    if (invert) {
        config->trigger = Trigger::OnRelease;
    } else {
        config->trigger = Trigger::OnPress;
    }

    auto remainingArgCount = args.size() - nextArgIndex;
    if (remainingArgCount == 1) {
        config->mode = MouseMode::Buttons;
        auto buttons = split(args[nextArgIndex], '+');
        for (auto &buttonName: buttons) {
            MouseButton button;
            if (buttonName == "left") {
                button = MouseButton::Left;
            } else if (buttonName == "right") {
                button = MouseButton::Right;
            } else if (buttonName == "middle") {
                button = MouseButton::Middle;
            } else if (buttonName == "forward") {
                button = MouseButton::Forward;
            } else if (buttonName == "back") {
                button = MouseButton::Back;
            } else {
                std::cerr << "Unknown button " << buttonName << std::endl;
                printSetMouseHelp(name);
                return {};
            }

            if (config->buttons.count(button) > 0) {
                std::cerr << buttonName << " button already specified" << std::endl;
                return {};
            }

            config->buttons.insert(button);
        }

        if (config->buttons.empty()) {
            std::cerr << "At least one button must be provided. Alternatively, enter mouse movements" << std::endl;
            printSetMouseHelp(name);
            return {};
        }
    } else {
        config->mode = MouseMode::Axis;

        if (remainingArgCount < 2) {
            printSetMouseHelp(name);
            return {};
        }

        auto xMovement = parseInt(args[nextArgIndex + 0]);
        auto yMovement = parseInt(args[nextArgIndex + 1]);

        if (!xMovement || (*xMovement < -100 || *xMovement > 100)) {
            std::cerr << "Invalid mouse X value. Expected a value between -100 and 100" << std::endl;
            return {};
        }
        if (!yMovement || (*yMovement < -100 || *yMovement > 100)) {
            std::cerr << "Invalid mouse Y value. Expected a value between -100 and 100" << std::endl;
            return {};
        }

        config->relativeX = static_cast<int8_t>(*xMovement);
        config->relativeY = static_cast<int8_t>(*yMovement);

        if (remainingArgCount >= 3) {
            auto wheel = parseInt(args[nextArgIndex + 2]);

            if (!wheel || (*wheel < -100 || *wheel > 100)) {
                std::cerr << "Invalid mouse wheel value. Expected a value between -100 and 100" << std::endl;
                return {};
            }

            config->wheelDelta = static_cast<int8_t>(*wheel);
        }
    }

    return config;
}
