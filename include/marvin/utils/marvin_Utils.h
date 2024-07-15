// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_UTILS_H
#define MARVIN_UTILS_H
#include <marvin/library/marvin_Literals.h>
#include <span>
#include <string>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

namespace marvin::utils {

    /**
        Writes the contents of an array-like to a CSV file. The resulting CSV will be single dimensional, with each element as a column.
        \param path The full path to the destination file.
        \param data The data to write to the destination file.
    */
    template <typename T>
    void writeToCsv(const std::string& path, std::span<T> data) {
        std::stringstream stream;
        for (auto i = 0_sz; i < data.size(); ++i) {
            stream << data[i];
            if (i != data.size() - 1) {
                stream << ",";
            }
        }
        const auto asStr = stream.str();
        std::ofstream outStream{ path, std::ios::out };
        outStream << asStr;
        outStream.flush();
    }

    /**
        Attemps to retrieve the full path to the current executable file.
        \return An optional containing the full path to the current executable file if no errors occured, std::nullopt otherwise.
    */
    [[nodiscard]] std::optional<std::string> getCurrentExecutablePath();

    bool readBinaryFile(const std::string& path, std::vector<char>& data);

} // namespace marvin::utils
#endif
