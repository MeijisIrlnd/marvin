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
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::math {

    template <FloatType T>
    [[nodiscard]] T lerp(T start, T end, T ratio) noexcept {
        const auto interpolated = start + (end - start) * ratio;
        return interpolated;
    }

} // namespace marvin::math
#include "marvin/library/marvin_DisableWarnings.h"

#endif
