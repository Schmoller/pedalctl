#pragma once

#include "../configuration/base.hpp"
#include "../configuration/keyboard.hpp"

#define PACKED __attribute__ ((packed))

enum Pedal {
    PE_LEFT = 1,
    PE_MIDDLE = 2,
    PE_RIGHT = 3,
};

enum ModifierKeys : unsigned char {
    MK_LEFT_CONTROL = 0x01,
    MK_LEFT_SHIFT = 0x02,
    MK_LEFT_ALT = 0x04,
    MK_LEFT_SUPER = 0x08,
    MK_RIGHT_CONTROL = 0x10,
    MK_RIGHT_SHIFT = 0x20,
    MK_RIGHT_ALT = 0x40,
    MK_RIGHT_SUPER = 0x80,
};

enum MouseButton : unsigned char {
    MB_LEFT = 0x01,
    MB_RIGHT = 0x02,
    MB_MIDDLE = 0x04,
    MB_BACK = 0x08,
    MB_FORWARD = 0x10,
};

enum MediaButton : unsigned char {
    MEB_VOLUME_MINUS = 1,
    MEB_VOLUME_PLUS,
    MEB_MUTE,
    MEB_PLAY,
    MEB_FORWARD,
    MEB_NEXT,
    MEB_STOP,
    MEB_OPEN_PLAYER,
    MEB_OPEN_HOMEPAGE,
    MEB_STOP_WEBPAGE,
    MEB_BACK_BROWSE,
    MEB_FORWARD_BROWSE,
    MEB_REFRESH,
    MEB_OPEN_MY_COMPUTER,
    MEB_OPEN_MAIL,
    MEB_OPEN_CALC,
    MEB_OPEN_SEARCH,
    MEB_SHUTDOWN,
    MEB_SLEEP,
};

enum GameKey : unsigned char {
    GK_LEFT = 1,
    GK_RIGHT,
    GK_UP,
    GK_DOWN,
    GK_BUTTON_1,
    GK_BUTTON_2,
    GK_BUTTON_3,
    GK_BUTTON_4,
    GK_BUTTON_5,
    GK_BUTTON_6,
    GK_BUTTON_7,
    GK_BUTTON_8,
};

enum TriggerMode : unsigned char {
    TM_RELEASE,
    TM_PRESS
};

enum ConfigType : unsigned char {
    CT_KEYBOARD_MULTI = 0x01,
    CT_KEYBOARD_SINGLE = 0x81,
    CT_MOUSE = 0x02,
    CT_MEDIA = 0x07,
    CT_GAME = 0x08,
};

enum PacketType : unsigned char {
    PT_CONFIG = 0x08,
    PT_STRING_CONFIG = 0x10
};

struct PACKED ConfigPacket {
    PacketType packetType;
    ConfigType type;

    union {
        struct PACKED {
            char modifiers;
            char keys[5];
        } keyboard;
        struct PACKED {
            char unknown[2];
            char buttons;
            char mouseX;
            char mouseY;
            char mouseWheel;
        } mouse;
        struct PACKED {
            MediaButton key;
            char padding[5];
        } media;
        struct PACKED {
            GameKey key;
            char padding[5];
        } game;
    };
};

SharedConfiguration parseConfig(const ConfigPacket &packet);
SharedConfiguration parseKeyboardConfig(const ConfigPacket &packet);
