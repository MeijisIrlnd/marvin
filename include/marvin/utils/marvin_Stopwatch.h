// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================
#ifndef MARVIN_STOPWATCH_H
#define MARVIN_STOPWATCH_H

#include <chrono>
#include <vector>
namespace marvin::utils {
    class Stopwatch final {
    public:
        Stopwatch() = default;
        explicit Stopwatch(size_t toReserve);
        Stopwatch(const Stopwatch& other) = delete;
        Stopwatch(Stopwatch&& other) noexcept = delete;
        Stopwatch& operator=(const Stopwatch& other) = delete;
        Stopwatch& operator=(Stopwatch&& other) = delete;
        ~Stopwatch() noexcept = default;

        void start();
        void stop();

        void write(const std::string& destFile);

    private:
        bool m_isMeasuring{ false };
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint{};
        std::vector<double> m_cpuTimes{};
    };
} // namespace marvin::utils
#endif