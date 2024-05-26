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
#include <mach-o/dyld.h>
#elif defined(MARVIN_WINDOWS)
#include <windows.h>
#elif defined(MARVIN_LINUX)
#include <filesystem>
#include <system_error>
#endif
namespace marvin::utils {
    std::optional<std::string> getCurrentExecutablePath() {
        [[maybe_unused]] constexpr static auto maxLength{ 512 };
#if defined(MARVIN_MACOS)
        char data[maxLength];
        std::uint32_t length{ sizeof(data) };
        const auto resCode = _NSGetExecutablePath(data, &length);
        if (resCode != 0) return {};
        std::string res = data;
        return res;
#elif defined(MARVIN_WINDOWS)
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
        std::error_code errorCode;
        auto pathRes = std::filesystem::canonical("/proc/self/exe", errorCode);
        if (errorCode) return {};
        auto strPath = pathRes.string();
        return strPath;
#else
        return {};
#endif
    }

} // namespace marvin::utils