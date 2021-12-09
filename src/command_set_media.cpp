#include "command_set.hpp"
#include "configuration/media.hpp"
#include "utils/string_utils.hpp"
#include <iostream>

void printSetMediaHelp(const std::string_view &name) {
    std::cerr
        << "Usage: " << name << " set DEVICE PEDAL media [OPTIONS] KEY" << std::endl
        << std::endl
        << "  Sets a pedal to control a multi-media key." << std::endl
        << std::endl
        << "ARGUMENTS" << std::endl
        << "  DEVICE\t\tThe device index to configure" << std::endl
        << "  PEDAL\t\t\tThe pedal name or index" << std::endl
        << "  KEY\t\t\tThe key name. See below"
        << std::endl
        << "OPTIONS" << std::endl
        << "  -i, --invert\t\tInverts pedal activation. The key will be pressed when" << std::endl
        << "  \t\t\tthe pedal is released." << std::endl
        << "KEY" << std::endl
        << "  Available multi-media keys:" << std::endl
        << "    decrease_volume, increase_volume, mute, play, forward, next, stop" << std::endl
        << "    open_player, open_homepage, stop_webpage, navigate_forward, navigate_back" << std::endl
        << "    refresh, open_my_computer, open_mail, open_search, shutdown, sleep" << std::endl

        << std::endl;
}


std::optional<SharedConfiguration> parseSetMediaOptions(
    const std::string_view &name, const std::vector<std::string_view> &args
) {
    if (args.empty()) {
        printSetMediaHelp(name);
        return {};
    }

    bool invert = false;

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

        if (arg == "-i" || arg == "--invert") {
            if (invert) {
                std::cerr << "--invert already set. This flag can only be specified once" << std::endl;
                printSetMediaHelp(name);
                return {};
            }
            invert = true;
        } else {
            std::cerr << "Unknown option " << arg << std::endl;
            printSetMediaHelp(name);
            return {};
        }
    }

    if (nextArgIndex >= args.size()) {
        std::cerr << "Missing key" << std::endl;
        printSetMediaHelp(name);
        return {};
    }

    auto config = std::make_shared<MediaConfiguration>();
    auto &key = args[nextArgIndex];

    if (invert) {
        config->trigger = Trigger::OnRelease;
    } else {
        config->trigger = Trigger::OnPress;
    }

    if (key == "decrease_volume") {
        config->button = MultiMediaButton::DecreaseVolume;
    } else if (key == "increase_volume") {
        config->button = MultiMediaButton::IncreaseVolume;
    } else if (key == "mute") {
        config->button = MultiMediaButton::Mute;
    } else if (key == "play") {
        config->button = MultiMediaButton::Play;
    } else if (key == "forward") {
        config->button = MultiMediaButton::Forward;
    } else if (key == "next") {
        config->button = MultiMediaButton::Next;
    } else if (key == "stop") {
        config->button = MultiMediaButton::Stop;
    } else if (key == "open_player") {
        config->button = MultiMediaButton::OpenPlayer;
    } else if (key == "open_homepage") {
        config->button = MultiMediaButton::OpenHomepage;
    } else if (key == "stop_webpage") {
        config->button = MultiMediaButton::StopWebPage;
    } else if (key == "navigate_forward") {
        config->button = MultiMediaButton::NavigateForward;
    } else if (key == "navigate_back") {
        config->button = MultiMediaButton::NavigateBack;
    } else if (key == "refresh") {
        config->button = MultiMediaButton::Refresh;
    } else if (key == "open_my_computer") {
        config->button = MultiMediaButton::OpenMyComputer;
    } else if (key == "open_mail") {
        config->button = MultiMediaButton::OpenMail;
    } else if (key == "open_search") {
        config->button = MultiMediaButton::OpenSearch;
    } else if (key == "shutdown") {
        config->button = MultiMediaButton::Shutdown;
    } else if (key == "sleep") {
        config->button = MultiMediaButton::Sleep;
    } else {
        std::cerr << "Unknown media key " << key << std::endl;
        printSetMediaHelp(name);
        return {};
    }

    return config;
}
