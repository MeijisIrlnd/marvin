// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_LITERALS_H
#define MARVIN_LITERALS_H
#include <cstddef>
namespace marvin {
    inline namespace literals {
        constexpr size_t operator""_sz(size_t x) {
            return x;
        }
    } // namespace literals
} // namespace marvin
#endif