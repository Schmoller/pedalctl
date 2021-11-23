#pragma once

#include <libusb.h>

class USBInterfaceLock {
public:
    explicit USBInterfaceLock(libusb_device_handle *, int interface);
    ~USBInterfaceLock();

private:
    libusb_device_handle *handle;
    int interface;
};



