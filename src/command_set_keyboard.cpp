#include "command_set.hpp"
#include "configuration/keyboard.hpp"
#include "configuration/keys.hpp"
#include "utils/string_utils.hpp"
#include <iostream>

void printSetKeyboardHelp(const std::string_view &name) {
    std::cerr
        << "Usage: " << name << " set DEVICE PEDAL keyboard [OPTIONS] KEYS" << std::endl
        << std::endl
        << "  Sets a pedal to act as a keyboard." << std::endl
        << std::endl
        << "  With no options, the pedal acts as a standard keyboard key." << std::endl
        << "  When pressed, the key will trigger. When released, the key will release." << std::endl
        << "  Keys are repeated the same as normal keyboard keys as long as the pedal" << std::endl
        << "  is pressed." << std::endl
        << std::endl
        << "ARGUMENTS" << std::endl
        << "  DEVICE\t\tThe device index to configure" << std::endl
        << "  PEDAL\t\t\tThe pedal name or index" << std::endl
        << std::endl
        << "OPTIONS" << std::endl
        << "  -i, --invert\t\tInverts pedal activation. The key will trigger as long " << std::endl
        << "  \t\t\tas the pedal is not pressed. In ONCE mode, the key will trigger " << std::endl
        << "  \t\t\twhen the pedal is released" << std::endl
        << "  -o, --once\t\tThe key activates only once (press and release) at the " << std::endl
        << "  \t\t\tactivation point" << std::endl
        << std::endl
        << "KEYS" << std::endl
        << "  One or more keys may be provided. All will be pressed at the same time." << std::endl
        << "  Multiple keys can be separated with +. For example: lcontrol+s" << std::endl
        << std::endl
        << "  Number and letter keys may be specified as is (lowercase only). Symbols and" << std::endl
        << "  modifier keys have special names as given below:" << std::endl
        << std::endl
        << "  Modifier Keys:" << std::endl
        << "    " << KeyName::LeftControl << "\t\tLeft control key" << std::endl
        << "    " << KeyName::LeftAlt << "\t\tLeft alt key" << std::endl
        << "    " << KeyName::LeftShift << "\t\tLeft shift key" << std::endl
        << "    " << KeyName::LeftSuper << "\t\tLeft super / windows / gui key" << std::endl
        << "    " << KeyName::RightControl << "\t\tRight control key" << std::endl
        << "    " << KeyName::RightAlt << "\t\tRight alt key" << std::endl
        << "    " << KeyName::RightShift << "\t\tRight shift key" << std::endl
        << "    " << KeyName::RightSuper << "\t\tRight super / windows / gui key" << std::endl
        << std::endl
        << "  Special Keys:" << std::endl
        << "    " << KeyName::Space << ", " << KeyName::Return << ", " << KeyName::Escape << ", "
        << KeyName::Delete << ", " << KeyName::Tab << ", " << KeyName::CapsLock << ", "
        << KeyName::NumLock << ", " << KeyName::ScrollLock << ", " << std::endl
        << "    " << KeyName::Home << ", " << KeyName::End << ", " << KeyName::Insert << ", " << KeyName::Pause << ", "
        << KeyName::Up << ", " << KeyName::Down << ", " << KeyName::Left << ", " << KeyName::Right << ", "
        << KeyName::PageUp << ", " << KeyName::PageDown << ", " << KeyName::PrintScreen
        << std::endl;
}


std::optional<SharedConfiguration> parseSetKeyboardOptions(
    const std::string_view &name, const std::vector<std::string_view> &args
) {
    if (args.empty()) {
        printSetKeyboardHelp(name);
        return {};
    }

    bool invert = false;
    bool once = false;

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
                printSetKeyboardHelp(name);
                return {};
            }
            invert = true;
        } else if (arg == "-o" || arg == "--once") {
            if (once) {
                std::cerr << "--once already set. This flag can only be specified once" << std::endl;
                printSetKeyboardHelp(name);
                return {};
            }
            once = true;
        } else {
            std::cerr << "Unknown option " << arg << std::endl;
            printSetKeyboardHelp(name);
            return {};
        }
    }

    if (nextArgIndex >= args.size()) {
        std::cerr << "Missing keys" << std::endl;
        printSetKeyboardHelp(name);
        return {};
    }

    auto &keys = args[nextArgIndex];

    auto keyboard = std::make_shared<KeyboardConfiguration>();
    if (once) {
        keyboard->mode = KeyMode::OneShot;
    } else {
        keyboard->mode = KeyMode::Standard;
    }

    if (invert) {
        keyboard->trigger = Trigger::OnRelease;
    } else {
        keyboard->trigger = Trigger::OnPress;
    }

    keyboard->keys = split(keys, '+');

    int nonModifierCount = 0;
    for (auto &key: keyboard->keys) {
        if (!isValidKey(key)) {
            std::cerr << "Unknown key " << key << std::endl;
            return {};
        }
        if (!isModifierKey(key)) {
            ++nonModifierCount;
        }
    }

    if (nonModifierCount > 6) {
        std::cerr << "Too many non-modifier keys. Only 6 keys may be given at once" << std::endl;
        return {};
    }

    return keyboard;
}
