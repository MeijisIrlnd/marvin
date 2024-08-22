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
#include <iterator>
#include <type_traits>
#include <cstddef>
#include <complex>
#include <concepts>
#include <span>
#include <vector>
namespace marvin {

    /**
        \brief Contrains T to be either a float or a double.
    */
    template <class T>
    concept FloatType = std::is_floating_point_v<T>;

    template <class T>
    concept ComplexFloatType = std::same_as<T, std::complex<float>> || std::same_as<T, std::complex<double>>;

    template <class T>
    concept RealOrComplexFloatType = FloatType<T> || ComplexFloatType<T>;

    /**
        \brief Constrains T to be any numeric type.
    */
    template <class T>
    concept NumericType = std::is_integral_v<T> || std::is_floating_point_v<T>;

    /**
        \brief Constrains T to be a fixed width signed int.
     */
    template <class T>
    concept FixedWidthSignedInteger = std::same_as<std::int8_t, T> ||
                                      std::same_as<std::int16_t, T> ||
                                      std::same_as<std::int32_t, T> ||
                                      std::same_as<std::int64_t, T>;


    /**
        \brief Constrains T to a type that defines a const_iterator as a child, has an implementation of `operator[](size_t)` and has a `.size()` member.
    */
    template <class T>
    concept ArrayLike = requires(T a, size_t i) {
        typename T::value_type;
        typename T::iterator;
        requires std::contiguous_iterator<typename T::iterator>;
        requires !std::same_as<std::string, T>;
        { a.size() } -> std::same_as<size_t>;
        { a[i] } -> std::same_as<typename T::value_type&>;
        { a.data() } -> std::same_as<typename T::value_type*>;
    };

    /**
        \brief Constrains T to be an Array like, with a Float-like value type.
    */
    template <class T>
    concept FloatArrayLike = requires {
        requires ArrayLike<T>;
        requires FloatType<typename T::value_type>;
    };

    /**
        \brief Constrains T to a class that implements `get()`, `reset()` `operator*()` and `operator->()`.
    */
    template <class T>
    concept SmartPointerType = requires(T a) {
        typename T::element_type;
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
