#include "Utility.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>

void Utility::ZipStrings(std::string& buffer, const std::string& s1, const std::string& s2) {
    std::istringstream ss1(s1);
    std::istringstream ss2(s2);
    std::ostringstream result;

    std::string line1, line2;
    bool firstLine = true;

    while (std::getline(ss1, line1) && std::getline(ss2, line2)) {
        if (!firstLine) result << '\n';
        result << line1 << ' ' << line2;
        firstLine = false;
    }

    buffer = result.str();
}

size_t Utility::CountLines(const std::string& str) {
    if (str.empty()) return 0;

    size_t lines = 0;
    for (char c : str) {
        if (c == '\n') ++lines;
    }
    // Add 1 if the string doesn't end with a newline
    if (str.back() != '\n') ++lines;

    return lines;
}

std::unordered_map<std::string, std::string> Utility::ParseArgs(int argc, char* argv[]) {
    std::unordered_map<std::string, std::string> args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg.rfind("--", 0) == 0) {  // starts with "--"
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                // Format: --key=value
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                args[key] = value;
            } else {
                // Flag without value
                std::string key = arg.substr(2);
                args[key] = "";  // empty value
            }
        }
    }

    return args;
}

std::vector<std::string> Utility::Split(const std::string& s, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        parts.push_back(item);
    }

    return parts;
}

bool Utility::StringToUint8(const std::string& str, uint8_t& out) {
    try {
        int value = std::stoi(str);
        if (value < 0 || value > 255) {
            return false;
        }
        out = static_cast<uint8_t>(value);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}