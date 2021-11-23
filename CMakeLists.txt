cmake_minimum_required(VERSION 3.20)
project(pedalctl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

include_directories(libs)

find_package(LibUsb REQUIRED)
include_directories(${LIBUSB_1_INCLUDE_DIRS})

add_executable(pedalctl src/main.cpp src/command_list.cpp src/command_show.cpp src/devices/ikkegol_pedal.cpp src/utils/usb_interface_lock.cpp)

target_link_libraries(pedalctl ${LIBUSB_1_LIBRARIES})
