#include "commands.hpp"
#include "devices/ikkegol_pedal.hpp"
#include "configuration/keyboard.hpp"
#include "configuration/text.hpp"
#include "configuration/mouse.hpp"
#include "configuration/gamepad.hpp"
#include "configuration/media.hpp"
#include <iostream>

void printConfig(SharedConfiguration config);
void printKeyboardConfig(KeyboardConfiguration &config);
void printTextConfig(TextConfiguration &config);
void printMouseConfig(MouseConfiguration &config);
void printGamepadConfig(GamepadConfiguration &config);
void printMediaConfig(MediaConfiguration &config);

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
            printMouseConfig(static_cast<MouseConfiguration &>(*config));
            break;
        }
        case ConfigurationType::Text: {
            printTextConfig(static_cast<TextConfiguration &>(*config));
            break;
        }
        case ConfigurationType::Media: {
            printMediaConfig(static_cast<MediaConfiguration &>(*config));
            break;
        }
        case ConfigurationType::Gamepad: {
            printGamepadConfig(static_cast<GamepadConfiguration &>(*config));
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

void printMouseConfig(MouseConfiguration &config) {
    if (config.mode == MouseMode::Buttons) {
        std::cout << "  Buttons: ";
        bool first = true;
        for (auto button: config.buttons) {
            if (!first) {
                std::cout << " + ";
            }
            first = false;
            switch (button) {
                case MouseButton::Left:
                    std::cout << "left";
                    break;
                case MouseButton::Right:
                    std::cout << "right";
                    break;
                case MouseButton::Middle:
                    std::cout << "middle";
                    break;
                case MouseButton::Back:
                    std::cout << "back";
                    break;
                case MouseButton::Forward:
                    std::cout << "forward";
                    break;
            };
        }
        std::cout << std::endl;
    } else {
        std::cout << "  Mouse move: " << (int) config.relativeX << "," << (int) config.relativeY << std::endl;
        std::cout << "  Mouse wheel: " << (int) config.wheelDelta << std::endl;
    }
}

void printGamepadConfig(GamepadConfiguration &config) {
    std::cout << "  Button: ";
    switch (config.button) {
        case GamepadButton::Left:
            std::cout << "Left";
            break;
        case GamepadButton::Right:
            std::cout << "Right";
            break;
        case GamepadButton::Up:
            std::cout << "Up";
            break;
        case GamepadButton::Down:
            std::cout << "Down";
            break;
        case GamepadButton::Button1:
            std::cout << "Button 1";
            break;
        case GamepadButton::Button2:
            std::cout << "Button 2";
            break;
        case GamepadButton::Button3:
            std::cout << "Button 3";
            break;
        case GamepadButton::Button4:
            std::cout << "Button 4";
            break;
        case GamepadButton::Button5:
            std::cout << "Button 5";
            break;
        case GamepadButton::Button6:
            std::cout << "Button 6";
            break;
        case GamepadButton::Button7:
            std::cout << "Button 7";
            break;
        case GamepadButton::Button8:
            std::cout << "Button 8";
            break;
    };
    std::cout << std::endl;
}

void printMediaConfig(MediaConfiguration &config) {
    std::cout << "  Button: ";
    switch (config.button) {
        case MultiMediaButton::DecreaseVolume:
            std::cout << "decrease_volume";
            break;
        case MultiMediaButton::IncreaseVolume:
            std::cout << "increase_volume";
            break;
        case MultiMediaButton::Mute:
            std::cout << "mute";
            break;
        case MultiMediaButton::Play:
            std::cout << "play";
            break;
        case MultiMediaButton::Forward:
            std::cout << "forward";
            break;
        case MultiMediaButton::Next:
            std::cout << "next";
            break;
        case MultiMediaButton::Stop:
            std::cout << "stop";
            break;
        case MultiMediaButton::OpenPlayer:
            std::cout << "open_player";
            break;
        case MultiMediaButton::OpenHomepage:
            std::cout << "open_homepage";
            break;
        case MultiMediaButton::StopWebPage:
            std::cout << "stop_web_page";
            break;
        case MultiMediaButton::NavigateBack:
            std::cout << "navigate_back";
            break;
        case MultiMediaButton::NavigateForward:
            std::cout << "navigate_forward";
            break;
        case MultiMediaButton::Refresh:
            std::cout << "refresh";
            break;
        case MultiMediaButton::OpenMyComputer:
            std::cout << "open_my_computer";
            break;
        case MultiMediaButton::OpenMail:
            std::cout << "open_mail";
            break;
        case MultiMediaButton::OpenCalc:
            std::cout << "open_calculator";
            break;
        case MultiMediaButton::OpenSearch:
            std::cout << "open_search";
            break;
        case MultiMediaButton::Shutdown:
            std::cout << "shutdown";
            break;
        case MultiMediaButton::Sleep:
            std::cout << "sleep";
            break;
    };
    std::cout << std::endl;
}
