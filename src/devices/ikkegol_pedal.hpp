#pragma once

#include "../configuration/base.hpp"
#include "ikkegol_capabilities.hpp"
#include <vector>
#include <string>
#include <memory>
#include <vector>
#include <libusb.h>

class IkkegolPedal {
public:
    explicit IkkegolPedal(libusb_device *);
    ~IkkegolPedal();

    bool isValid() const { return handle != nullptr; }

    const std::string &getModel() const { return model; }

    const std::string &getVersion() const { return version; }

    const std::string &getId() const { return id; }

    bool load();

    uint32_t getPedalCount() const { return capabilities.pedals; }

    SharedConfiguration getConfiguration(uint32_t pedal) const;
private:
    libusb_device_handle *handle {};
    std::string model;
    std::string version;
    std::string id;
    Capabilities capabilities;
    std::vector<SharedConfiguration> pedalConfiguration;

    void init();
    bool readModelAndVersion();
    bool readPedals();
    SharedConfiguration readConfiguration(uint32_t pedal);
};

typedef std::shared_ptr<IkkegolPedal> SharedIkkegolPedal;

std::vector<SharedIkkegolPedal> discoverIkkegolDevices();
SharedIkkegolPedal findIkkegolDevice(const std::string_view &id);

