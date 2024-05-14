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
#include <cstddef>
namespace marvin {
    /**
        \brief Contrains T to be either a float or a double.
    */
    template <class T>
    concept FloatType = requires {
        std::is_floating_point_v<T>;
    };


    /**
        \brief Constrains T to be any numeric type.
    */
    template <class T>
    concept NumericType = requires {
        std::is_integral_v<T> ||
            std::is_floating_point_v<T>;
    };


    /**
        \brief Constrains T to a type that defines a const_iterator as a child, has an implementation of `operator[](size_t)` and has a `.size()` member.
    */
    template <class T>
    concept ArrayLike = requires(T a) {
        typename T::iterator;
        typename T::value_type;
        a.operator[](0);
        a.data();
        a.size();
    };

    /**
        \brief
    */
    template <class T>
    concept FloatArrayLike = requires {
        ArrayLike<T>;
        FloatType<typename T::value_type>;
    };

    /**
        \brief Constrains T to a class that implements `get()`, `reset()` `operator*()` and `operator->()`.
    */
    template <class T>
    concept SmartPointerType = requires(T a) {
        a.get();
        a.reset();
        a.operator*();
        a.operator->();
    };


    /**
        Checks if `N` is a power of two.
        \returns `true` if N is a power of two, otherwise `false`.
    */
    template <size_t N>
    constexpr bool isPowerOfTwo() {
        if constexpr (N == 1) {
            return true;
        } else if constexpr (N == 0 || N % 2 != 0) {
            return false;
        } else {
            return isPowerOfTwo<N / 2>();
        }
    }
} // namespace marvin
#endif
