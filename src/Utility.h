#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <iostream>

namespace Utility {

    /** Concatenate two strings line by line. */
    void ZipStrings(std::string& buffer, const std::string& s1, const std::string& s2);

    /** Count howmany lines a string has. */
    size_t CountLines(const std::string& str);

    /** Move the cursor down n lines */
    void CursorDown(std::ostream& stream, size_t lines);

    /** Move the cursor to a specific row and column. */
    void CursorPos(std::ostream& stream, size_t row, size_t col);

    /** Parse commandline arguments to be easyer to handle */
    std::unordered_map<std::string, std::string> ParseArgs(int argc, char* argv[]);

    /** Split a string by delimiter. */
    std::vector<std::string> Split(const std::string& s, char delimiter);

    /** Convert a string to uint8_t */
    bool StringToUint8(const std::string& str, uint8_t& out);


}
