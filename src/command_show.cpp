#include "commands.hpp"

void showCommand(args::Subparser &parser) {
    args::Positional<std::string> device(parser, "device", "Device ID of the pedal", args::Options::Required);

    parser.Parse();

}
