// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_CONVERSIONS_H
#define MARVIN_CONVERSIONS_H
#include <marvin/library/marvin_Concepts.h>
namespace marvin::math {

    /**
     * Converts a time in milliseconds to seconds
     * \param ms Time in milliseconds.
     * \return Time in seconds.
     */
    template <FloatType T>
    [[nodiscard]] T msToSeconds(T ms) noexcept {
        return ms / static_cast<T>(1000.0);
    }

    /**
     * Converts a time in seconds to samples.
     * \param seconds Time in seconds.
     * \param sampleRate The currently used sample rate.
     * \return The number of samples for the given time in seconds.
     */
    template <FloatType T>
    [[nodiscard]] T secondsToSamples(T seconds, double sampleRate) noexcept {
        return seconds * static_cast<T>(sampleRate);
    }

    /**
     * Converts a time in milliseconds to samples.
     * \param ms Time in milliseconds.
     * \param sampleRate The currently used sample rate.
     * \return The number of samples for the given time in milliseconds.
     */
    template <FloatType T>
    [[nodiscard]] T msToSamples(T ms, double sampleRate) noexcept {
        const FloatType auto seconds = msToSeconds(ms);
        return secondsToSamples(seconds, sampleRate);
    }

    /**
        Converts from decibels to gain.
        \param db The level in decibels.
        \param referenceMinDb The level in decibels that should correspond to 0 gain. Optional, defaults to -100dB.
        \return The level in decibels converted to a 0 to 1 gain.
    */
    template <FloatType T>
    [[nodiscard]] T dbToGain(T db, T referenceMinDb = static_cast<T>(-100.0)) {
        if (db > referenceMinDb) {
            return std::pow(static_cast<T>(10.0), db * static_cast<T>(0.05));
        } else {
            return static_cast<T>(0.0);
        }
    }

    /**
        Converts from gain to decibels.
        \param gain The 0 to 1 gain to convert.
        \param minusInfDb The level in decibels that should correspond to 0 gain. Optional, defaults to -100dB.
        \return The converted level in decibels.
    */
    template <FloatType T>
    [[nodiscard]] T gainToDb(T gain, T minusInfDb = static_cast<T>(-100.0)) noexcept {
        const auto clamped = std::clamp(gain, static_cast<T>(0.0), static_cast<T>(1.0));
        if (clamped <= 0.0f) return minusInfDb;
        const auto db = std::max(static_cast<T>(20.0) * std::log10(clamped), minusInfDb);
        return db;
    }

} // namespace marvin::math

#endif // INFERNO_MARVIN_CONVERSIONS_H
