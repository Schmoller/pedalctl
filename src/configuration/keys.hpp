#pragma once

#include <string>

#define KEY(name, id) constexpr const char* name = #id

namespace KeyName {
KEY(LeftControl, lcontrol);
KEY(RightControl, rcontrol);
KEY(LeftAlt, lalt);
KEY(RightAlt, ralt);
KEY(LeftShift, lshift);
KEY(RightShift, rshift);
KEY(LeftSuper, lsuper);
KEY(RightSuper, rsuper);
KEY(Space, space);
KEY(Return, return);
KEY(Escape, escape);
KEY(Delete, delete);
KEY(Tab, tab);
KEY(CapsLock, caps_lock);
KEY(NumLock, num_lock);
KEY(ScrollLock, scroll_lock);
KEY(Home, home);
KEY(End, end);
KEY(Insert, insert);
KEY(Pause, pause);
KEY(Up, up);
KEY(Down, down);
KEY(Left, left);
KEY(Right, right);
KEY(PageUp, page_up);
KEY(PageDown, page_down);
KEY(PrintScreen, print_screen);
}

bool isValidKey(const std::string_view &name);
bool isModifierKey(const std::string_view &name);

#undef KEY
