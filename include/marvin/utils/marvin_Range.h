// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_RANGE_H
#define MARVIN_RANGE_H
#include "marvin/library/marvin_Concepts.h"
namespace marvin::utils {
    /**
        \brief POD type that represents a range of values, for classes requiring a min and a max.

        Literally just two T-s - mainly exists to avoid needing to use `std::tuple`.
    */
    template <NumericType T>
    struct Range {
        T min;
        T max;

        /**
         * Calculates the midpoint between `min` and `max`.
         * \return The midpoint between `min` and `max`.
         */
        [[nodiscard]] T midpoint() const noexcept {
            const auto midPoint = (max - min) / static_cast<T>(2.0);
            return midPoint;
        }
    };

    /**
        Equality operator overload for Range<T>.
        \param lhs a const lvalue ref of type Range<T>
        \param rhs a const lvalue ref of type Range<T>
        \return true if lhs.min == rhs.min, and lhs.max == rhs.max, false otherwise.
    */
    template <NumericType T>
    bool operator==(const Range<T>& lhs, const Range<T>& rhs) {
        return (lhs.min == rhs.min) && (lhs.max == rhs.max);
    }

    /**
        Inequality operator overload for Range<T>
        \param lhs A const lvalue ref of type Range<T>.
        \param rhs A const rvalue ref of type Range<T>.
        \return true if lhs.min != rhs.min or lhs.max != rhs.max, false otherwise.
    */
    template <NumericType T>
    bool operator!=(const Range<T>& lhs, const Range<T>& rhs) {
        return !(lhs == rhs);
    }
} // namespace marvin::utils
#endif