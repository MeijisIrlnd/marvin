// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================
#include <marvin/utils/marvin_Utils.h>

#if defined(MARVIN_MACOS)

#elif defined(MARVIN_WINDOWS)
#include <windows.h>
#elif defined(MARVIN_LINUX)

#endif
namespace marvin::utils {
    std::optional<std::string> getCurrentExecutablePath() {
#if defined(MARVIN_MACOS)
        return {};
#elif defined(MARVIN_WINDOWS)
        constexpr static auto maxLength{ 512 };
        char data[maxLength];
        auto dest = (LPSTR)data;
        GetModuleFileName(nullptr, dest, maxLength);
        std::string cpy = dest;
        const auto lastErr = GetLastError();
        if (lastErr != ERROR_SUCCESS) {
            return {};
        }
        return cpy;
#elif defined(MARVIN_LINUX)
        return {};
#else
        return {};
#endif
    }

} // namespace marvin::utils