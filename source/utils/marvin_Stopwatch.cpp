// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/utils/marvin_Stopwatch.h>
#include <marvin/utils/marvin_Utils.h>

namespace marvin::utils {

    Stopwatch::Stopwatch(size_t toReserve) {
        m_cpuTimes.reserve(toReserve);
    }

    void Stopwatch::start() {
        if (m_isMeasuring) return;
        m_isMeasuring = true;
        m_startTimePoint = std::chrono::high_resolution_clock::now();
    }

    void Stopwatch::stop() {
        if (!m_isMeasuring) return;
        m_isMeasuring = false;
        const auto endPoint = std::chrono::high_resolution_clock::now();
        const auto delta = std::chrono::duration_cast<std::chrono::microseconds>(endPoint - m_startTimePoint);
        const auto asDbl = static_cast<double>(delta.count());
        m_cpuTimes.emplace_back(asDbl);
    }

    void Stopwatch::write(const std::string& destFile) {
        writeToCsv<double>(destFile, m_cpuTimes);
    }
} // namespace marvin::utils