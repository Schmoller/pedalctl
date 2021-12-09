#include "command_set.hpp"
#include "configuration/keyboard.hpp"
#include "configuration/keys.hpp"
#include "utils/string_utils.hpp"
#include "configuration/text.hpp"
#include <iostream>

void printSetTextHelp(const std::string_view &name) {
    std::cerr
        << "Usage: " << name << " set DEVICE PEDAL text [OPTIONS] TEXT" << std::endl
        << std::endl
        << "  Sets a pedal to type some text when pressed." << std::endl
        << std::endl
        << "  You can specify up to 38 characters." << std::endl
        << std::endl
        << "ARGUMENTS" << std::endl
        << "  DEVICE\t\tThe device index to configure" << std::endl
        << "  PEDAL\t\t\tThe pedal name or index" << std::endl
        << std::endl
        << "OPTIONS" << std::endl
        << "  -i, --invert\t\tInverts pedal activation. The text will be typed " << std::endl
        << "  \t\t\twhen the pedal is released." << std::endl
        << std::endl;
}


std::optional<SharedConfiguration> parseSetTextOptions(
    const std::string_view &name, const std::vector<std::string_view> &args
) {
    if (args.empty()) {
        printSetTextHelp(name);
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
                printSetTextHelp(name);
                return {};
            }
            invert = true;
        } else {
            std::cerr << "Unknown option " << arg << std::endl;
            printSetTextHelp(name);
            return {};
        }
    }

    if (nextArgIndex >= args.size()) {
        std::cerr << "Missing text" << std::endl;
        printSetTextHelp(name);
        return {};
    }

    std::string remaining;
    for (auto argIndex = nextArgIndex; argIndex < args.size(); ++argIndex) {
        auto &arg = args[argIndex];
        if (argIndex != nextArgIndex) {
            remaining.push_back(' ');
        }
        remaining.append(arg);
    }

    if (remaining.size() > 38) {
        std::cerr << "Warning: You can only have up to 38 characters. You have " << remaining.size() << std::endl;
        std::cerr << "         Only the first 38 characters will be used" << std::endl;
        remaining = remaining.substr(0, 38);
    }

    auto config = std::make_shared<TextConfiguration>();
    config->text = remaining;

    if (invert) {
        config->trigger = Trigger::OnRelease;
    } else {
        config->trigger = Trigger::OnPress;
    }

    return config;
}
