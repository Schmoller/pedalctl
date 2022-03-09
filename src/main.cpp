#include "commands.hpp"
#include <iostream>
#include <libusb.h>
#include <string>
#include <vector>

int parseOptions(const std::string_view &name, const std::vector<std::string_view> &);

int main(int argc, char **argv) {
    std::vector<std::string_view> commandLine(argc - 1);
    for (auto index = 1; index < argc; ++index) {
        commandLine[index - 1] = std::string_view(argv[index]);
    }

    std::string_view name = argv[0];
    auto lastSlash = name.find_last_of('/');
    if (lastSlash != std::string_view::npos) {
        name = name.substr(lastSlash + 1);
    }

    auto result = libusb_init(nullptr);
    if (result < 0) {
        std::cerr << "Failed to initialize libusb. Error: " << libusb_error_name(result) << std::endl;
        return 1;
    }

    auto exitCode = parseOptions(name, commandLine);

    libusb_exit(nullptr);

    return exitCode;
}

void printHelp(const std::string_view &name) {
    std::cerr
        << "Usage: " << name << " [OPTIONS] COMMAND { ARGS | help }" << std::endl
        << std::endl
        << "OPTIONS" << std::endl
        << "  -h, --help\t\tShows this help" << std::endl
        << "  -v, --version\t\tShows the version" << std::endl
        << std::endl
        << "COMMAND" << std::endl
        << "  list\t\tLists all supported pedal devices" << std::endl
        << "  show\t\tShows the current configuration of a device" << std::endl
        << "  set\t\tChanges the configuration of a device" << std::endl
        << std::endl;
}

void printVersion() {
    std::cerr << "PedalCtl 0.2" << std::endl;
}

int parseOptions(const std::string_view &name, const std::vector<std::string_view> &args) {
    if (args.empty()) {
        printHelp(name);
        return 1;
    }

    size_t nextArgIndex;
    // Options first
    for (nextArgIndex = 0; nextArgIndex < args.size(); ++nextArgIndex) {
        auto &arg = args[nextArgIndex];

        if (arg.empty()) {
            continue;
        }

        // No more options after this
        if (arg == "--" || arg[0] != '-') {
            break;
        }

        if (arg == "-h" || arg == "--help") {
            printHelp(name);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else {
            std::cerr << "Unknown option " << arg << std::endl;
            printHelp(name);
            return 1;
        }
    }

    if (nextArgIndex >= args.size()) {
        std::cerr << "Missing command" << std::endl;
        printHelp(name);
        return 1;
    }

    // Process command
    auto &commandName = args[nextArgIndex];
    std::vector<std::string_view> commandArgs { args.begin() + static_cast<long>(nextArgIndex + 1), args.end() };

    if (commandName == "list") {
        return listCommand(name, commandArgs);
    } else if (commandName == "show") {
        return showCommand(name, commandArgs);
    } else if (commandName == "set") {
        return setCommand(name, commandArgs);
    } else {
        std::cerr << "Unknown command " << commandName << std::endl;
        printHelp(name);
        return 1;
    }
}