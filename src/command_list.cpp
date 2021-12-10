#include "commands.hpp"
#include "devices/ikkegol_pedal.hpp"
#include <iostream>

void printListHelp(const std::string_view &name) {
    std::cerr
        << "Usage: " << name << " list [help]" << std::endl
        << std::endl
        << "  List all available pedal devices." << std::endl
        << std::endl;
}

int listCommand(const std::string_view &name, const std::vector<std::string_view> &args) {
    if (!args.empty()) {
        if (args[0] == "help") {
            printListHelp(name);
            return 0;
        } else {
            std::cerr << "Unknown sub-command " << args[0] << std::endl;
            printListHelp(name);
            return 1;
        }
    }

    auto devices = discoverIkkegolDevices();
    if (devices.empty()) {
        std::cout << "No devices detected" << std::endl;
        return 0;
    }
    std::cout << devices.size() << " discovered devices" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << std::endl;

    for (auto &device: devices) {
        std::cout << " " << device->getId() << ": ";

        if (device->isValid()) {
            std::cout << device->getModel() << " Version " << device->getVersion() << std::endl;
        } else {
            std::cout << "* Cannot read device - " << device->getLastError() << std::endl;
        }
    }

    return 0;
}
