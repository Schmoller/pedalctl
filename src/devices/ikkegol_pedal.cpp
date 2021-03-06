#include "ikkegol_pedal.hpp"
#include "../utils/usb_interface_lock.hpp"
#include "ikkegol_protocol.hpp"
#include "../configuration/keyboard.hpp"
#include "../utils/errors.hpp"
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
            devices.push_back(footPedal);
            ++nextId;
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
            if (footPedal->getId() == id) {
                found = footPedal;
                break;
            }
            ++nextId;
        }
    }

    libusb_free_device_list(list, 1);

    return found;
}

IkkegolPedal::IkkegolPedal(libusb_device *device, int id) : id(id) {
    auto result = libusb_open(device, &handle);
    updateLastError(result);

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

    pedalModified.clear();
    pedalModified.resize(capabilities.pedals);
    pedalTriggerTypeModified.clear();
    pedalTriggerTypeModified.resize(capabilities.pedals);
    std::fill(pedalModified.begin(), pedalModified.end(), false);
    std::fill(pedalTriggerTypeModified.begin(), pedalTriggerTypeModified.end(), false);
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
        updateLastError(result);
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
        pedalConfiguration[pedal] = readConfiguration(pedal + capabilities.firstPedalIndex);
    }

    std::fill(pedalModified.begin(), pedalModified.end(), false);
    std::fill(pedalTriggerTypeModified.begin(), pedalTriggerTypeModified.end(), false);

    if (!readPedalTriggerModes()) {
        return false;
    }

    return true;
}

const SharedConfiguration IkkegolPedal::getConfiguration(uint32_t pedal) const {
    if (pedal < pedalConfiguration.size()) {
        return pedalConfiguration[pedal];
    }
    return {};
}

void IkkegolPedal::setConfiguration(uint32_t pedal, const SharedConfiguration &config) {
    assert(pedal < pedalConfiguration.size());
    assert(config);

    auto &oldConfig = pedalConfiguration[pedal];
    if (oldConfig && oldConfig->trigger != config->trigger) {
        pedalTriggerTypeModified[pedal] = true;
    }

    pedalConfiguration[pedal] = config;
    pedalModified[pedal] = true;
}

bool IkkegolPedal::readPedalTriggerModes() {
    uint8_t request[8] = { 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    int wrote;
    auto result = libusb_interrupt_transfer(
        handle, ConfigEndpoint | LIBUSB_ENDPOINT_OUT, request, sizeof(request), &wrote, 100
    );
    if (wrote < 0 || result < 0) {
        updateLastError(result);
        return false;
    }

    uint8_t buffer[16];
    int read;
    result = libusb_interrupt_transfer(handle, ConfigEndpoint | LIBUSB_ENDPOINT_IN, buffer, 8, &read, 100);
    if (result < 0) {
        updateLastError(result);
        return false;
    }

    if (buffer[0] > 8) {
        result = libusb_interrupt_transfer(handle, ConfigEndpoint | LIBUSB_ENDPOINT_IN, &buffer[8], 8, &read, 100);
        if (result < 0) {
            updateLastError(result);
            return false;
        }
    }

    for (auto pedal = 0; pedal < capabilities.pedals; ++pedal) {
        auto mode = static_cast<TriggerMode>(buffer[pedal + capabilities.firstPedalIndex + 1]);
        auto &config = pedalConfiguration[pedal];

        if (!config) {
            // Pedal not-configured
            continue;
        }

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
        updateLastError(result);
        return {};
    }

    uint8_t buffer[40];
    std::fill_n(buffer, sizeof(buffer), 0);

    int read;
    result = libusb_interrupt_transfer(handle, ConfigEndpoint | LIBUSB_ENDPOINT_IN, buffer, 8, &read, 100);
    if (result < 0) {
        updateLastError(result);
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
                updateLastError(result);
                return {};
            }
        }
    }

    auto *packet = reinterpret_cast<ConfigPacket *>(buffer);
    return parseConfig(*packet);
}

bool IkkegolPedal::save() {
    bool anyModified = false;
    for (auto modified: pedalModified) {
        anyModified = anyModified || modified;
    }

    if (!anyModified) {
        return true;
    }

    USBInterfaceLock interfaceLock(handle, ConfigInterface);

    if (!beginWrite()) {
        return false;
    }

    for (auto pedal = 0; pedal < capabilities.pedals; ++pedal) {
        if (!pedalModified[pedal]) {
            continue;
        }

        if (!writeConfiguration(pedal + capabilities.firstPedalIndex, pedalConfiguration[pedal])) {
            return false;
        }
    }

    bool anyTriggerModified = false;
    for (auto modified: pedalTriggerTypeModified) {
        anyTriggerModified = anyTriggerModified || modified;
    }

    if (anyTriggerModified) {
        if (!writePedalTriggerModes()) {
            return false;
        }
    }

    std::fill(pedalModified.begin(), pedalModified.end(), false);
    std::fill(pedalTriggerTypeModified.begin(), pedalTriggerTypeModified.end(), false);
    readPedalTriggerModes();
    return true;
}

bool IkkegolPedal::beginWrite() {
    uint8_t request[8] = { 0x01, 0x80, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00 };

    int wrote;
    auto result = libusb_interrupt_transfer(
        handle, ConfigEndpoint | LIBUSB_ENDPOINT_OUT, request, sizeof(request), &wrote, 100
    );
    if (wrote < 0 || result < 0) {
        updateLastError(result);
        return false;
    }

    return true;
}

bool IkkegolPedal::writeConfiguration(uint32_t pedal, const SharedConfiguration &config) {
    assert(config);

    auto packet = encodeConfigPacket(config);

    uint8_t requestInitiate[8] = { 0x01, 0x81, packet.size, static_cast<uint8_t>(pedal + 1), 0x00, 0x00, 0x00, 0x00 };

    int wrote;
    auto result = libusb_interrupt_transfer(
        handle, ConfigEndpoint | LIBUSB_ENDPOINT_OUT, requestInitiate, sizeof(requestInitiate), &wrote, 100
    );
    if (wrote < 0 || result < 0) {
        updateLastError(result);
        return false;
    }

    auto *requestBody = reinterpret_cast<uint8_t *>(&packet);

    auto pages = ((packet.size + 7) & ~7) >> 3;
    for (auto page = 0; page < pages; ++page) {
        result = libusb_interrupt_transfer(
            handle, ConfigEndpoint | LIBUSB_ENDPOINT_OUT, &requestBody[page * 8], 8, &wrote, 100
        );
        if (wrote < 0 || result < 0) {
            updateLastError(result);
            return false;
        }
    }

    return true;
}

bool IkkegolPedal::writePedalTriggerModes() {
    auto payloadSize = static_cast<uint8_t>(capabilities.pedals + capabilities.firstPedalIndex + 1);
    uint8_t requestInitiate[8] = {
        0x01, 0x85, payloadSize, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    int wrote;
    auto result = libusb_interrupt_transfer(
        handle, ConfigEndpoint | LIBUSB_ENDPOINT_OUT, requestInitiate, sizeof(requestInitiate), &wrote, 100
    );
    if (wrote < 0 || result < 0) {
        updateLastError(result);
        return false;
    }

    uint8_t buffer[16] {};
    buffer[0] = payloadSize;

    for (auto pedal = 0; pedal < capabilities.pedals; ++pedal) {
        auto &config = pedalConfiguration[pedal];
        if (config) {
            if (config->trigger == Trigger::OnPress) {
                buffer[1 + pedal + capabilities.firstPedalIndex] = TM_PRESS;
            } else if (config->trigger == Trigger::OnRelease) {
                buffer[1 + pedal + capabilities.firstPedalIndex] = TM_RELEASE;
            } else {
                buffer[1 + pedal + capabilities.firstPedalIndex] = TM_PRESS;
            }
        } else {
            buffer[1 + pedal + capabilities.firstPedalIndex] = TM_PRESS;
        }
    }

    auto pages = ((payloadSize + 7) & ~7) >> 3;
    for (auto page = 0; page < pages; ++page) {
        result = libusb_interrupt_transfer(
            handle, ConfigEndpoint | LIBUSB_ENDPOINT_OUT, &buffer[page * 8], 8, &wrote, 100
        );
        if (wrote < 0 || result < 0) {
            updateLastError(result);
            return false;
        }
    }

    return true;
}

std::string_view IkkegolPedal::getPedalName(uint32_t pedal) const {
    assert(pedal < capabilities.pedals);

    if (capabilities.pedalNames == nullptr) {
        return {};
    }

    return capabilities.pedalNames[pedal];
}

void IkkegolPedal::updateLastError(int result) {
    if (result < 0) {
        lastError = describeLibUSBError(result);
    }
}
