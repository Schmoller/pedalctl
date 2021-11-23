#include <iostream>
#include <args.hxx>
#include <libusb.h>
#include "commands.hpp"

const char *ProgramDescription = "A command line application for configuring foot pedal input devices";
const char *ListDescription = "Lists all available devices";
const char *ShowDescription = "Shows the current configuration of a device";

int main(int argc, char **argv) {
    args::ArgumentParser parser(ProgramDescription);

    args::HelpFlag help(parser, "help", "Display's this information", { 'h', "help" });

    args::Group commands(parser, "commands");
    args::Command list(commands, "list", ListDescription, &listCommand);
    args::Command show(commands, "show", ShowDescription, &showCommand);

    auto result = libusb_init(nullptr);
    if (result < 0) {
        std::cerr << "Failed to initialize libusb. Error: " << libusb_error_name(result) << std::endl;
        return 1;
    }

    try {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help &) {
        std::cout << parser;
    }
    catch (args::Error &e) {
        std::cerr << e.what() << std::endl << parser;
        return 1;
    }

    libusb_exit(nullptr);

    return 0;
}
