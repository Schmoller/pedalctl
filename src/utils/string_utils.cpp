#include "string_utils.hpp"

std::vector<std::string> split(const std::string_view &source, char delimiter) {
    std::vector<std::string> output;

    size_t start = 0;
    for (;;) {
        auto end = source.find(delimiter, start);
        output.emplace_back(source.substr(start, end - start));
        if (end == std::string::npos) {
            break;
        }

        start = end + 1;
    };

    return output;
}
