// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_MATH_H
#define MARVIN_MATH_H
#include "marvin/library/marvin_Concepts.h"
#include <cmath>
namespace marvin::math {

    /**
        Returns a point `ratio` of the way between `start` and `end`.
        \param start The start value of the interpolation.
        \param end The end value of the interpolation.
        \param ratio The (0 to 1) position between `start` and `end`
        \return A linear interpolation between `start` and `end` at position `ratio`.
    */
    template <FloatType T>
    [[nodiscard]] T lerp(T start, T end, T ratio) noexcept {
        const auto interpolated = start + (end - start) * ratio;
        return interpolated;
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

} // namespace marvin::math

#endif
