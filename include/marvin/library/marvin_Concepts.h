// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_CONCEPTS_H
#define MARVIN_CONCEPTS_H
#include <type_traits>
namespace marvin {
    template <class T>
    concept FloatType = requires {
        std::is_floating_point_v<T>;
    };


    template <class T>
    concept NumericType = requires {
        std::is_integral_v<T> ||
            std::is_floating_point_v<T>;
    };

    template <class T>
    concept SmartPointerType = requires(T a) {
        a.get();
        a.reset();
        a.operator*();
        a.operator->();
    };
} // namespace marvin
#endif
