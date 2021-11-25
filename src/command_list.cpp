#include "commands.hpp"
#include "devices/ikkegol_pedal.hpp"
#include <iostream>

void listCommand(args::Subparser &parser) {
    parser.Parse();

    auto devices = discoverIkkegolDevices();
    if (devices.empty()) {
        std::cout << "No devices detected" << std::endl;
        return;
    }
    std::cout << devices.size() << " discovered devices" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << std::endl;

    for (auto &device: devices) {
        std::cout << " " << device->getId() << ": " << device->getModel() << std::endl;
    }
}
