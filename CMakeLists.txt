cmake_minimum_required(VERSION 3.20)
project(pedalctl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

include_directories(libs)

find_package(LibUsb REQUIRED)
include_directories(${LIBUSB_1_INCLUDE_DIRS})

add_executable(pedalctl
        src/main.cpp
        src/command_list.cpp
        src/command_show.cpp
        src/command_set.cpp
        src/command_set_keyboard.cpp
        src/command_set_mouse.cpp
        src/command_set_text.cpp
        src/command_set_media.cpp
        src/command_set_game.cpp
        src/devices/ikkegol_pedal.cpp
        src/utils/usb_interface_lock.cpp
        src/devices/ikkegol_protocol.cpp
        src/devices/ikkegol_capabilities.cpp
        src/utils/string_utils.cpp
        src/utils/usb_scancodes.cpp
        src/configuration/keys.cpp
        src/configuration/dumper.cpp
        src/utils/command_line.cpp
        src/utils/errors.cpp
        )

target_link_libraries(pedalctl ${LIBUSB_1_LIBRARIES})
