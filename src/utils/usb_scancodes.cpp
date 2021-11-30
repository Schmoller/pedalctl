#include "usb_scancodes.hpp"

int scanCodeFromKey(const std::string_view &name) {
    auto size = sizeof(ScanCodeNames) / sizeof(const char *);
    for (auto index = 0; index < size; ++index) {
        auto otherName = ScanCodeNames[index];
        if (otherName == nullptr) {
            continue;
        }

        if (name == otherName) {
            return index;
        }
    }

    return -1;
}
