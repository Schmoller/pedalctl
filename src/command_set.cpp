#include "commands.hpp"
#include "devices/ikkegol_pedal.hpp"
#include "configuration/keyboard.hpp"
#include "configuration/text.hpp"
#include "configuration/mouse.hpp"
#include "configuration/gamepad.hpp"
#include "configuration/media.hpp"
#include "utils/string_utils.hpp"
#include "utils/usb_scancodes.hpp"
#include <iostream>

void setCommand(args::Subparser &parser) {
    args::Positional<uint32_t> deviceId(parser, "<device>", "Device ID of the pedal", args::Options::Required);
    args::Positional<std::string> pedal(parser, "<pedal>", "The name or index of the pedal", args::Options::Required);

    // args::Group settings(parser, "settings");
    // args::Command keyboard(settings, "keyboard", "Configures the pedal to be one or more keyboard keys");
    // args::Flag keyboardOneShot(keyboard, "once", "Turns off key repeat", { "once" });

    //  args::Command mouse(settings, "mouse", "Configures the pedal to be one or more mouse buttons or movement");

    args::Flag onRelease(
        parser, "onrelease", "Triggers the action on pedal release instead of press.", { "on-release" }
    );

    args::Group settings(parser, "settings", args::Group::Validators::Xor);
    args::ValueFlag<std::string> keys(
        settings, "keys", "Sets one or more keyboard keys. Use + to combine.", { "keyboard" }
    );
    args::Flag once(settings, "once", "Limits keyboard keys to fire once. Key repeat is disabled", { "no-repeat" });

    args::ValueFlag<std::string> mouseButtons(
        settings, "buttons", "Sets one or more mouse buttons. Use + to combine.", { "mouse" }
    );

    parser.Parse();

    auto device = findIkkegolDevice(deviceId.Get());
    if (!device) {
        std::cerr << "Unable to find device " << deviceId.Get() << std::endl;
        exit(1);
    }

    SharedConfiguration config;
    if (keys) {
        auto keyboard = std::make_shared<KeyboardConfiguration>();
        if (once.Get()) {
            keyboard->mode = KeyMode::OneShot;
        } else {
            keyboard->mode = KeyMode::Standard;
        }

        keyboard->keys = split(keys.Get(), '+');

        int nonModifierCount = 0;
        for (auto &key: keyboard->keys) {
            if (!isValidKey(key)) {
                std::cerr << "Unknown key " << key << std::endl;
                exit(1);
            }
            if (!isModifierKey(key)) {
                ++nonModifierCount;
            }
        }

        if (nonModifierCount > 6) {
            std::cerr << "Too many non-modifier keys. Only 6 keys may be given at once" << std::endl;
            exit(1);
        }

        config = keyboard;
    }

    if (onRelease.Get()) {
        config->trigger = Trigger::OnRelease;
    } else {
        config->trigger = Trigger::OnPress;
    }

    if (!device->load()) {
        std::cerr << "Unable to read configuration" << std::endl;
        exit(1);
    }

    // TODO: Parse the pedal number
    device->setConfiguration(0, config);

    if (!device->save()) {
        std::cerr << "Unable to write configuration" << std::endl;
        exit(1);
    }

    std::cout << "Updated configuration" << std::endl;
}