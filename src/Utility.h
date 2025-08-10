#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <vector>

namespace Utility {

    /** Concatenate two strings line by line. */
    void ZipStrings(std::string& buffer, const std::string& s1, const std::string& s2);

    /** Convert a RGB value to hex string. */
    std::string RGBtoHEX(uint8_t r, uint8_t g, uint8_t b);

    /** Convert a HEX string to RGB color. */
    bool HEXtoRGB(const std::string& hex, uint8_t& out_r, uint8_t& out_g, uint8_t& out_b);

    /** Count howmany lines a string has. */
    size_t CountLines(const std::string& str);

    /** Parse commandline arguments to be easyer to handle */
    std::unordered_map<std::string, std::string> ParseArgs(int argc, char* argv[]);

    /** Split a string by delimiter. */
    std::vector<std::string> Split(const std::string& s, char delimiter);

    /** Convert a string to uint8_t */
    bool StringToUint8(const std::string& str, uint8_t& out);


}