#include "keys.hpp"
#include "../utils/usb_scancodes.hpp"

bool isValidKey(const std::string_view &name) {
    if (scanCodeFromKey(name) >= 0) {
        return true;
    }
    if (isModifierKey(name)) {
        return true;
    }

    return false;
}

bool isModifierKey(const std::string_view &name) {
    if (name == KeyName::LeftControl) {
        return true;
    }
    if (name == KeyName::LeftAlt) {
        return true;
    }
    if (name == KeyName::LeftShift) {
        return true;
    }
    if (name == KeyName::LeftSuper) {
        return true;
    }
    if (name == KeyName::RightControl) {
        return true;
    }
    if (name == KeyName::RightAlt) {
        return true;
    }
    if (name == KeyName::RightShift) {
        return true;
    }
    if (name == KeyName::RightSuper) {
        return true;
    }

    return false;
}
