#pragma once

#include "../configuration/keys.hpp"
#include <string>
#include <array>

constexpr const char *ScanCodeNames[] = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "g",
    "h",
    "i",
    "j",
    "k",
    "l",
    "m",
    "n",
    "o",
    "p",
    "q",
    "r",
    "s",
    "t",
    "u",
    "v",
    "w",
    "x",
    "y",
    "z",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    KeyName::Return,
    KeyName::Escape,
    KeyName::Delete,
    KeyName::Tab,
    KeyName::Space,
    "-",
    "=",
    "[",
    "]",
    "\\",
    nullptr,
    ";",
    "'",
    "`",
    ",",
    ".",
    "/",
    KeyName::CapsLock,
    "f1",
    "f2",
    "f3",
    "f4",
    "f5",
    "f6",
    "f7",
    "f8",
    "f9",
    "f10",
    "f11",
    "f12",
    KeyName::PrintScreen,
    KeyName::ScrollLock,
    KeyName::Pause,
    KeyName::Insert,
    KeyName::Home,
    KeyName::PageUp,
    KeyName::Delete,
    KeyName::End,
    KeyName::PageDown,
    KeyName::Right,
    KeyName::Left,
    KeyName::Down,
    KeyName::Up,
    KeyName::NumLock,
    "num/",
    "num*",
    "num-",
    "num+",
    "num_enter",
    "num1",
    "num2",
    "num3",
    "num4",
    "num5",
    "num6",
    "num7",
    "num8",
    "num9",
    "num0",
    "num.",
    nullptr,
    "num=",
    "f13",
    "f14",
    "f15",
    "f16",
    "f17",
    "f18",
    "f19",
    "f20",
    "f21",
    "f22",
    "f23",
    "f24",
};

constexpr const char *PrintableScanCodes[] = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    "aA",
    "bB",
    "cC",
    "dD",
    "eE",
    "fF",
    "gG",
    "hH",
    "iI",
    "jJ",
    "kK",
    "lL",
    "mM",
    "nN",
    "oO",
    "pP",
    "qQ",
    "rR",
    "sS",
    "tT",
    "uU",
    "vV",
    "wW",
    "xX",
    "yY",
    "zZ",
    "1!",
    "2@",
    "3#",
    "4$",
    "5%",
    "6^",
    "7&",
    "8*",
    "9(",
    "0)",
    "\n\n",
    nullptr,
    nullptr,
    "\t\t",
    "  ",
    "-_",
    "=+",
    "[{",
    "]}",
    "\\|",
    nullptr,
    ";:",
    "'\"",
    "`~",
    ",<",
    ".>",
    "/?",
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    "//",
    "**",
    "--",
    "++",
    "\n\n",
    "11",
    "22",
    "33",
    "44",
    "55",
    "66",
    "77",
    "88",
    "99",
    "00",
    "..",
    nullptr,
    "=="
};

constexpr auto generatePrintableReverseLut() {
    constexpr auto size = sizeof(PrintableScanCodes) / sizeof(const char *);
    constexpr uint8_t Shift = 0x80;

    std::array<int, 128> table { -1 };
    for (int ch = 0; ch < 128; ++ch) {
        for (auto scanCode = 0; scanCode < size; ++scanCode) {
            auto options = PrintableScanCodes[scanCode];
            if (options == nullptr) {
                continue;
            }

            if (options[0] == ch) {
                table[ch] = scanCode;
                break;
            } else if (options[1] == ch) {
                table[ch] = scanCode | Shift;
                break;
            }
        }
    }

    return table;
}

constexpr auto PrintableCharsToScancodes = generatePrintableReverseLut();

constexpr const char *scanCodeToKey(int scanCode) {
    if (scanCode < 0 || scanCode > sizeof(ScanCodeNames)) {
        return nullptr;
    }

    return ScanCodeNames[scanCode];
}

constexpr char scanCodeToPrintable(int scanCode, bool shift) {
    if (scanCode < 0 || scanCode > sizeof(PrintableScanCodes) || !PrintableScanCodes[scanCode]) {
        return '\0';
    }

    return PrintableScanCodes[scanCode][shift ? 1 : 0];
}

int scanCodeFromKey(const std::string_view &name);

constexpr int scanCodeFromPrintable(char ch) {
    if (ch < 0) {
        return -1;
    }

    return PrintableCharsToScancodes[ch];
}
