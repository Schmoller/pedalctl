#pragma once

#include <vector>
#include <string>
#include <memory>
#include <libusb.h>

class IkkegolPedal {
public:
    explicit IkkegolPedal(libusb_device *);
    ~IkkegolPedal();

    bool isValid() const { return handle != nullptr; }

    const std::string &getModel() const { return model; }

private:
    libusb_device_handle *handle {};
    std::string model;

    void init();
    bool readModel();
};

typedef std::shared_ptr<IkkegolPedal> SharedIkkegolPedal;

std::vector<SharedIkkegolPedal> discoverIkkegolDevices();

