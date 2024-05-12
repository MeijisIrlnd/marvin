// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

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
    };
} // namespace marvin::utils