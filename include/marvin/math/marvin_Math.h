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
#include <algorithm>

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

    /**
        Takes a value in range `srcMin` to `srcMax`, normalises it, and rescales it to be in range
        `newMin` to `newMax`
        \param x The value to remap.
        \param srcMin The min of the value's original range.
        \param srcMax The max of the value's original range.
        \param newMin The new range's min.
        \param newMax The new range's max.
        \return The rescaled value.
    */
    template <FloatType T>
    [[nodiscard]] T remap(T x, T srcMin, T srcMax, T newMin, T newMax) {
        // (v - mn) / (mx - mn) == v_n
        const auto normalised = (x - srcMin) / (srcMax - srcMin);
        // (v_n . (mx - mn)) + mn = v
        const auto rescaled = (normalised * (newMax - newMin)) + newMin;
        return rescaled;
    }

} // namespace marvin::math

#endif
