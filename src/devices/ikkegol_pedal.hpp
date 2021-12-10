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
    explicit IkkegolPedal(libusb_device *, int id);
    ~IkkegolPedal();

    bool isValid() const { return handle != nullptr; }

    const std::string &getModel() const { return model; }

    const std::string &getVersion() const { return version; }

    const std::string &getLastError() const { return lastError; }

    int getId() const { return id; }

    std::string_view getPedalName(uint32_t pedal) const;

    bool load();
    bool save();

    uint32_t getPedalCount() const { return capabilities.pedals; }

    const SharedConfiguration getConfiguration(uint32_t pedal) const;
    void setConfiguration(uint32_t pedal, const SharedConfiguration &config);
private:
    libusb_device_handle *handle {};
    std::string model;
    std::string version;
    int id;
    Capabilities capabilities;
    std::vector<SharedConfiguration> pedalConfiguration;
    std::vector<bool> pedalModified;
    std::vector<bool> pedalTriggerTypeModified;

    std::string lastError;

    void init();
    bool readModelAndVersion();
    bool readPedalTriggerModes();
    bool beginWrite();
    SharedConfiguration readConfiguration(uint32_t pedal);
    bool writeConfiguration(uint32_t pedal, const SharedConfiguration &config);
    bool writePedalTriggerModes();

    void updateLastError(int result);
};

typedef std::shared_ptr<IkkegolPedal> SharedIkkegolPedal;

std::vector<SharedIkkegolPedal> discoverIkkegolDevices();
SharedIkkegolPedal findIkkegolDevice(uint32_t id);

