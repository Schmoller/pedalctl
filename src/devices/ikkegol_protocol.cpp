#include "ikkegol_protocol.hpp"
#include "../configuration/keys.hpp"
#include "../utils/usb_scancodes.hpp"

#define MODIFIER(name) (packet.keyboard.modifiers & name) != 0

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
            break;
        case CT_MEDIA:
            break;
        case CT_GAME:
            break;
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
