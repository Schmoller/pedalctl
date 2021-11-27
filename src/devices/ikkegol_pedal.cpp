#include "ikkegol_pedal.hpp"
#include "../utils/usb_interface_lock.hpp"
#include "ikkegol_protocol.hpp"
#include "../configuration/keyboard.hpp"
#include <cstring>
#include <chrono>
#include <thread>
#include <cassert>

const uint16_t VendorId = 0x1a86;
const uint16_t ProductId = 0xe026;
const int ConfigInterface = 1;
const uint8_t ConfigEndpoint = 0x02;

std::vector<SharedIkkegolPedal> discoverIkkegolDevices() {
    std::vector<SharedIkkegolPedal> devices;

    libusb_device **list;

    auto deviceCount = libusb_get_device_list(nullptr, &list);
    if (deviceCount < 0) {
        // TODO: it would be better to return an error here
        return {};
    }

    int nextId = 1;
    for (auto index = 0; index < deviceCount; ++index) {
        libusb_device *device = list[index];
        libusb_device_descriptor descriptor;
        if (libusb_get_device_descriptor(device, &descriptor) < 0) {
            continue;
        }
        if (descriptor.idVendor == VendorId && descriptor.idProduct == ProductId) {
            auto footPedal = std::make_shared<IkkegolPedal>(device, nextId);
            if (footPedal->isValid()) {
                devices.push_back(footPedal);
                ++nextId;
            }
        }
    }

    libusb_free_device_list(list, 1);

    return devices;
}

SharedIkkegolPedal findIkkegolDevice(uint32_t id) {
    libusb_device **list;
    SharedIkkegolPedal found;

    auto deviceCount = libusb_get_device_list(nullptr, &list);
    if (deviceCount < 0) {
        // TODO: it would be better to return an error here
        return {};
    }

    int nextId = 1;
    for (auto index = 0; index < deviceCount; ++index) {
        libusb_device *device = list[index];
        libusb_device_descriptor descriptor;
        if (libusb_get_device_descriptor(device, &descriptor) < 0) {
            continue;
        }
        if (descriptor.idVendor == VendorId && descriptor.idProduct == ProductId) {
            auto footPedal = std::make_shared<IkkegolPedal>(device, nextId);
            if (footPedal->isValid()) {
                if (footPedal->getId() == id) {
                    found = footPedal;
                    break;
                }
                ++nextId;
            }
        }
    }

    libusb_free_device_list(list, 1);

    return found;
}

IkkegolPedal::IkkegolPedal(libusb_device *device, int id) : id(id) {
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
    readModelAndVersion();
    auto caps = getModelCapabilities(model);
    if (caps) {
        capabilities = *caps;
    }

    pedalConfiguration.clear();
    pedalConfiguration.resize(capabilities.pedals);
}

bool IkkegolPedal::readModelAndVersion() {
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
    auto separator = decoded.find_last_of('_');
    if (separator == std::string::npos) {
        // Unexpected format
        return false;
    }
    model = decoded.substr(0, separator);

    if (decoded[separator + 1] == 'V') {
        ++separator;
    }

    version = decoded.substr(separator + 1);
    return true;
}

bool IkkegolPedal::load() {
    if (!isValid()) {
        return false;
    }
    USBInterfaceLock interfaceLock(handle, ConfigInterface);

    for (auto pedal = 0; pedal < capabilities.pedals; ++pedal) {
        pedalConfiguration[pedal] = readConfiguration(pedal);
    }

    if (!readPedalTriggerModes()) {
        return false;
    }

    return true;
}

SharedConfiguration IkkegolPedal::getConfiguration(uint32_t pedal) const {
    if (pedal < pedalConfiguration.size()) {
        return pedalConfiguration[pedal];
    }
    return {};
}

bool IkkegolPedal::readPedalTriggerModes() {
    uint8_t request[8] = { 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    int wrote;
    auto result = libusb_interrupt_transfer(
        handle, ConfigEndpoint | LIBUSB_ENDPOINT_OUT, request, sizeof(request), &wrote, 100
    );
    if (wrote < 0 || result < 0) {
        return false;
    }

    uint8_t buffer[16];
    int read;
    result = libusb_interrupt_transfer(handle, ConfigEndpoint | LIBUSB_ENDPOINT_IN, buffer, 8, &read, 100);
    if (result < 0) {
        return false;
    }

    if (buffer[0] > 8) {
        result = libusb_interrupt_transfer(handle, ConfigEndpoint | LIBUSB_ENDPOINT_IN, &buffer[8], 8, &read, 100);
        if (result < 0) {
            return false;
        }
    }

    for (auto pedal = 0; pedal < capabilities.pedals; ++pedal) {
        auto mode = static_cast<TriggerMode>(buffer[pedal + 1]);
        auto &config = pedalConfiguration[pedal];

        switch (mode) {
            case TM_RELEASE:
                config->trigger = Trigger::OnRelease;
                break;
            case TM_PRESS:
                config->trigger = Trigger::OnPress;
                break;
        };
    }

    return true;
}

SharedConfiguration IkkegolPedal::readConfiguration(uint32_t pedal) {
    uint8_t request[8] = { 0x01, 0x82, 0x08, static_cast<uint8_t>(pedal + 1), 0x00, 0x00, 0x00, 0x00 };

    int wrote;
    auto result = libusb_interrupt_transfer(
        handle, ConfigEndpoint | LIBUSB_ENDPOINT_OUT, request, sizeof(request), &wrote, 100
    );
    if (wrote < 0 || result < 0) {
        return {};
    }

    uint8_t buffer[40];
    std::fill_n(buffer, sizeof(buffer), 0);

    int read;
    result = libusb_interrupt_transfer(handle, ConfigEndpoint | LIBUSB_ENDPOINT_IN, buffer, 8, &read, 100);
    if (result < 0) {
        return {};
    }

    assert(buffer[0] <= 40);

    if (buffer[0] > 8) {
        auto pages = ((buffer[0] + 7) & ~7) >> 3;
        for (auto page = 1; page < pages; ++page) {
            result = libusb_interrupt_transfer(
                handle, ConfigEndpoint | LIBUSB_ENDPOINT_IN, &buffer[page * 8], 8, &read, 100
            );
            if (result < 0) {
                return {};
            }
        }
    }

    auto *packet = reinterpret_cast<ConfigPacket *>(buffer);
    return parseConfig(*packet);
}
