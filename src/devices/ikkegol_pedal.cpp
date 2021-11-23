#include "ikkegol_pedal.hpp"
#include "../utils/usb_interface_lock.hpp"
#include <cstring>
#include <chrono>
#include <thread>

const uint16_t VendorId = 0x1a86;
const uint16_t ProductId = 0xe026;
const int ConfigInterface = 1;
const uint8_t ConfigEndpoint = 0x02;

std::vector<SharedIkkegolPedal> discoverIkkegolDevices() {
    std::vector<SharedIkkegolPedal> devices;

    libusb_device **list;
    libusb_device *found = nullptr;

    auto deviceCount = libusb_get_device_list(nullptr, &list);
    if (deviceCount < 0) {
        // TODO: it would be better to return an error here
        return {};
    }

    for (auto index = 0; index < deviceCount; ++index) {
        libusb_device *device = list[index];
        libusb_device_descriptor descriptor;
        if (libusb_get_device_descriptor(device, &descriptor) < 0) {
            continue;
        }

        if (descriptor.idVendor == VendorId && descriptor.idProduct == ProductId) {
            auto footPedal = std::make_shared<IkkegolPedal>(device);
            if (footPedal->isValid()) {
                devices.push_back(footPedal);
            }
        }
    }

    libusb_free_device_list(list, 1);

    return devices;
}

IkkegolPedal::IkkegolPedal(libusb_device *device) {
    libusb_open(device, &handle);

    if (handle != nullptr) {
        init();
    }
}

IkkegolPedal::~IkkegolPedal() {
    if (handle) {
        libusb_close(handle);
    }
}

void IkkegolPedal::init() {
    libusb_set_auto_detach_kernel_driver(handle, 1);
    readModel();
}

bool IkkegolPedal::readModel() {
    constexpr uint32_t MaxAttempts = 10;
    constexpr uint32_t MaxSections = 4;

    USBInterfaceLock interfaceLock(handle, ConfigInterface);

    uint8_t request[8] = { 0x01, 0x83, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 };

    int wrote;
    auto result = libusb_interrupt_transfer(
        handle, ConfigEndpoint | LIBUSB_ENDPOINT_OUT, request, sizeof(request), &wrote, 100
    );
    if (wrote < 0 || result < 0) {
        return false;
    }

    uint8_t versionBuffer[32];
    uint8_t buffer[8];
    uint32_t sectionsRead = 0;
    uint32_t attempts = 0;

    do {
        int read;
        libusb_interrupt_transfer(handle, ConfigEndpoint | LIBUSB_ENDPOINT_IN, buffer, sizeof(buffer), &read, 100);
        if (read > 0) {
            std::memcpy(&versionBuffer[sectionsRead * 8], buffer, read);
            ++sectionsRead;
        } else if (read == 0 && sectionsRead > 0) {
            break;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        ++attempts;
    } while (sectionsRead < MaxSections && attempts < MaxAttempts);

    if (sectionsRead == 0) {
        return false;
    }

    std::string decoded(reinterpret_cast<char *>(versionBuffer), sectionsRead * 8);
    model = decoded;
    return true;
}
