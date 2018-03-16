#include "util.hpp"
#include <cstring>

std::vector<std::string> ir::split(std::string& str,
                                   const std::string& delimeters) {
    std::vector<std::string> result;

    // find the first token
    char* token = strtok(&str[0], delimeters.c_str());
    while (token != nullptr) {
        // end of the first token is replaced with \0 already.
        result.emplace_back(token);
        token = strtok(nullptr, delimeters.c_str());
    }

    return result;
}
