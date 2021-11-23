#include "usb_interface_lock.hpp"
#include <stdexcept>

USBInterfaceLock::USBInterfaceLock(libusb_device_handle *handle, int interface)
    : handle(handle), interface(interface) {
    auto result = libusb_claim_interface(handle, interface);
    if (result < 0) {
        throw std::runtime_error("Failed to claim interface");
    }
}

USBInterfaceLock::~USBInterfaceLock() {
    libusb_release_interface(handle, interface);
}
