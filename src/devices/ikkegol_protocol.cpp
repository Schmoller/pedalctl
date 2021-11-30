#include <cassert>
#include "ikkegol_protocol.hpp"
#include "../configuration/keys.hpp"
#include "../utils/usb_scancodes.hpp"
#include "../configuration/mouse.hpp"
#include "../configuration/gamepad.hpp"
#include "../configuration/media.hpp"
#include "../configuration/text.hpp"
#include "../configuration/keyboard.hpp"

#define MODIFIER(name) (packet.keyboard.modifiers & name) != 0
#define MOUSE_BUTTON(name) (packet.mouse.buttons & name) != 0

SharedConfiguration parseKeyboardConfig(const ConfigPacket &packet);
SharedConfiguration parseTextConfig(const ConfigPacket &packet);
SharedConfiguration parseMouseConfig(const ConfigPacket &packet);
SharedConfiguration parseGamepadConfig(const ConfigPacket &packet);
SharedConfiguration parseMediaConfig(const ConfigPacket &packet);

SharedConfiguration parseConfig(const ConfigPacket &packet) {
    SharedConfiguration configuration;
    switch (packet.type) {
        case CT_KEYBOARD:
        case CT_KEYBOARD_ONCE:
        case CT_KEYBOARD_MULTI:
        case CT_KEYBOARD_MULTI_ONCE:
            return parseKeyboardConfig(packet);
        case CT_TEXT:
            return parseTextConfig(packet);
        case CT_MOUSE:
            return parseMouseConfig(packet);
        case CT_MEDIA:
            return parseMediaConfig(packet);
        case CT_GAME:
            return parseGamepadConfig(packet);
    }

    return configuration;
}

SharedConfiguration parseKeyboardConfig(const ConfigPacket &packet) {
    auto config = std::make_shared<KeyboardConfiguration>();
    if (packet.type == CT_KEYBOARD_ONCE || packet.type == CT_KEYBOARD_MULTI_ONCE) {
        config->mode = KeyMode::OneShot;
    } else {
        config->mode = KeyMode::Standard;
    }

    std::vector<std::string> keys;
    if (MODIFIER(MK_LEFT_CONTROL)) {
        keys.emplace_back(KeyName::LeftControl);
    }
    if (MODIFIER(MK_RIGHT_CONTROL)) {
        keys.emplace_back(KeyName::RightControl);
    }
    if (MODIFIER(MK_LEFT_SHIFT)) {
        keys.emplace_back(KeyName::LeftShift);
    }
    if (MODIFIER(MK_RIGHT_SHIFT)) {
        keys.emplace_back(KeyName::RightShift);
    }
    if (MODIFIER(MK_LEFT_ALT)) {
        keys.emplace_back(KeyName::LeftAlt);
    }
    if (MODIFIER(MK_RIGHT_ALT)) {
        keys.emplace_back(KeyName::RightAlt);
    }
    if (MODIFIER(MK_LEFT_SUPER)) {
        keys.emplace_back(KeyName::LeftSuper);
    }
    if (MODIFIER(MK_RIGHT_SUPER)) {
        keys.emplace_back(KeyName::RightSuper);
    }

    for (auto index = 0; index < packet.size - offsetof(ConfigPacket, keyboard.keys); ++index) {
        auto key = packet.keyboard.keys[index];
        if (key == 0) {
            break;
        }

        auto name = scanCodeToKey(key);
        if (name != nullptr) {
            keys.emplace_back(name);
        }
    }

    config->keys = keys;
    return config;
}

SharedConfiguration parseTextConfig(const ConfigPacket &packet) {
    auto config = std::make_shared<TextConfiguration>();
    auto length = packet.size - offsetof(ConfigPacket, string.string);
    std::string text;

    for (auto index = 0; index < length; ++index) {
        int scanCode = packet.string.string[index];
        if (scanCode == 0) {
            break;
        }

        bool shift = (scanCode & 0x80) != 0;
        scanCode = scanCode & 0x7f;

        auto ch = scanCodeToPrintable(scanCode, shift);
        if (ch != '\0') {
            text.push_back(ch);
        }
    }

    config->text = text;
    return config;
}

SharedConfiguration parseMouseConfig(const ConfigPacket &packet) {
    auto config = std::make_shared<MouseConfiguration>();

    std::unordered_set<MouseButton> buttons;
    if (MOUSE_BUTTON(MB_LEFT)) {
        buttons.insert(MouseButton::Left);
    }
    if (MOUSE_BUTTON(MB_RIGHT)) {
        buttons.insert(MouseButton::Right);
    }
    if (MOUSE_BUTTON(MB_MIDDLE)) {
        buttons.insert(MouseButton::Middle);
    }
    if (MOUSE_BUTTON(MB_BACK)) {
        buttons.insert(MouseButton::Back);
    }
    if (MOUSE_BUTTON(MB_FORWARD)) {
        buttons.insert(MouseButton::Forward);
    }

    if (!buttons.empty()) {
        config->mode = MouseMode::Buttons;
        config->buttons = buttons;
    } else {
        config->mode = MouseMode::Axis;
        config->relativeX = packet.mouse.mouseX;
        config->relativeY = packet.mouse.mouseY;
        config->wheelDelta = packet.mouse.mouseWheel;
    }

    return config;
}

SharedConfiguration parseGamepadConfig(const ConfigPacket &packet) {
    auto config = std::make_shared<GamepadConfiguration>();

    auto button = static_cast<GamepadButton>(packet.game.key - 1);
    config->button = button;

    return config;
}

SharedConfiguration parseMediaConfig(const ConfigPacket &packet) {
    auto config = std::make_shared<MediaConfiguration>();

    auto button = static_cast<MultiMediaButton>(packet.media.key - 1);
    config->button = button;

    return config;
}

ConfigPacket encodeKeyboardPacket(const KeyboardConfiguration &config);
ConfigPacket encodeMousePacket(const MouseConfiguration &config);
ConfigPacket encodeTextPacket(const TextConfiguration &config);
ConfigPacket encodeMediaPacket(const MediaConfiguration &config);
ConfigPacket encodeGamepadPacket(const GamepadConfiguration &config);

ConfigPacket encodeConfigPacket(const SharedConfiguration &config) {
    switch (config->type()) {
        case ConfigurationType::Keyboard:
            return encodeKeyboardPacket(static_cast<KeyboardConfiguration &>(*config));
        case ConfigurationType::Mouse:
            return encodeMousePacket(static_cast<MouseConfiguration &>(*config));
        case ConfigurationType::Text:
            return encodeTextPacket(static_cast<TextConfiguration &>(*config));
        case ConfigurationType::Media:
            return encodeMediaPacket(static_cast<MediaConfiguration &>(*config));
        case ConfigurationType::Gamepad:
            return encodeGamepadPacket(static_cast<GamepadConfiguration &>(*config));
    }
}

ConfigPacket encodeKeyboardPacket(const KeyboardConfiguration &config) {
    ConfigPacket packet {};
    assert(!config.keys.empty());

    packet.keyboard.modifiers = 0;
    std::fill_n(packet.keyboard.keys, sizeof(packet.keyboard.keys), 0);
    auto index = 0;

    for (auto &key: config.keys) {
        if (key == KeyName::LeftControl) {
            packet.keyboard.modifiers |= MK_LEFT_CONTROL;
        } else if (key == KeyName::LeftShift) {
            packet.keyboard.modifiers |= MK_LEFT_SHIFT;
        } else if (key == KeyName::LeftAlt) {
            packet.keyboard.modifiers |= MK_LEFT_ALT;
        } else if (key == KeyName::LeftSuper) {
            packet.keyboard.modifiers |= MK_LEFT_SUPER;
        } else if (key == KeyName::RightControl) {
            packet.keyboard.modifiers |= MK_RIGHT_CONTROL;
        } else if (key == KeyName::RightShift) {
            packet.keyboard.modifiers |= MK_RIGHT_SHIFT;
        } else if (key == KeyName::RightAlt) {
            packet.keyboard.modifiers |= MK_RIGHT_ALT;
        } else if (key == KeyName::RightSuper) {
            packet.keyboard.modifiers |= MK_RIGHT_SUPER;
        } else {
            auto scanCode = scanCodeFromKey(key);
            assert(scanCode >= 0);
            assert(index < 6);
            packet.keyboard.keys[index++] = static_cast<int8_t>(scanCode);
        }
    }

    if (config.mode == KeyMode::Standard) {
        if (index > 1) {
            packet.type = CT_KEYBOARD_MULTI;
        } else {
            packet.type = CT_KEYBOARD;
        }
    } else {
        if (index > 1) {
            packet.type = CT_KEYBOARD_MULTI_ONCE;
        } else {
            packet.type = CT_KEYBOARD_ONCE;
        }
    }
    // NOTE: What happens if its just "lcontrol" or something like that?

    if (index == 1) {
        // This is an interesting quirk of these devices.
        packet.size = 8;
    } else {
        packet.size = 3 + index;
    }

    return packet;
}

ConfigPacket encodeMousePacket(const MouseConfiguration &config) {
    ConfigPacket packet {
        .type = CT_MOUSE,
    };
    // TODO: not implemented
    return packet;
}

ConfigPacket encodeTextPacket(const TextConfiguration &config) {
    ConfigPacket packet {
        .type = CT_TEXT,
    };
    // TODO: not implemented
    return packet;
}

ConfigPacket encodeMediaPacket(const MediaConfiguration &config) {
    ConfigPacket packet {
        .type = CT_MEDIA,
    };
    // TODO: not implemented
    return packet;
}

ConfigPacket encodeGamepadPacket(const GamepadConfiguration &config) {
    ConfigPacket packet {
        .type = CT_GAME,
    };
    // TODO: not implemented
    return packet;
}
