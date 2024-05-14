// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================
#ifndef MARVIN_SIMDOPS_H
#define MARVIN_SIMDOPS_H
#include <marvin/library/marvin_Concepts.h>
#include <marvin/library/marvin_Literals.h>
#include <type_traits>
#include <cstring>
#include <cassert>
/**
    \brief Collection of basic arithmetic operations on vectors, SIMD accelerated where possible.

    On macOS this will use Accelerate's vDSP library for SIMD intrinsics. On Windows, Marvin has an optional
    dependency on Intel's IPP library. If it's found by CMake, these functions will use the IPP implementations of
    these functions. If it's *not* found, it will use the fallback implementations, which are simple for loops. <br>
    If you're struggling to get IPP installed, Sudara has a great [blog post](https://melatonin.dev/blog/using-intel-performance-primitives-ipp-with-juce-and-cmake/)
    detailing the hoops you need to jump through to get it up and running. <br>

    Note that even when using SIMD,
    it's not guaranteed to be faster than what the compiler might generate with a for loop - I'd
    recommend benchmarking on the platform you're targeting with the kind of data you'll be using, and
    choosing whether to use these functions accordingly.
*/
namespace marvin::vecops {

    /**
        Adds the values of `rhs` to the values of `lhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void add(T* lhs, const T* rhs, size_t size) noexcept;
    /**
        Adds the value of `scalar` to the values of `lhs`,
        and stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to add to each element of `arr`.
        \param size The number of elements in `arr`.
    */
    template <FloatType T>
    void add(T* arr, T scalar, size_t size) noexcept;
    /**
        Adds the values of `rhs` to the values of `lhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatArrayLike T>
    void add(T& lhs, T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        add(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Adds the value of `scalar` to the values of `lhs`,
        and stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to add to each element of `arr`.
    */
    template <FloatArrayLike T>
    void add(T& arr, typename T::value_type scalar) noexcept {
        add(arr.data(), scalar, arr.size());
    }

    /**
        Subtracts the values of `rhs` from the values of `lhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void subtract(T* lhs, const T* rhs, size_t size) noexcept;
    /**
        Subtracts the value of `scalar` from the values of `lhs`, and
        stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to subtract from each element in `arr`.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void subtract(T* arr, T scalar, size_t size) noexcept;

    /**
        Subtracts the values of `rhs` from the values of `lhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatArrayLike T>
    requires std::is_floating_point_v<typename T::value_type>
    void subtract(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        subtract(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Subtracts the value of `scalar` from the values of `lhs`, and
        stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to subtract from each element in `arr`.
    */
    template <FloatArrayLike T>
    void subtract(T& arr, typename T::value_type scalar) noexcept {
        subtract(arr.data(), scalar, arr.size());
    }

    /**
        Multiplies the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void multiply(T* lhs, const T* rhs, size_t size) noexcept;

    /**
        Multiplies the values of `lhs` by value of `scalar`, and
        stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to multiply each element in `arr` by.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void multiply(T* arr, T scalar, size_t size) noexcept;

    /**
        Multiplies the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatArrayLike T>
    void multiply(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        multiply(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Multiplies the values of `lhs` by the value of `scalar`, and
        stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to multiply each element in `arr` by.
    */
    template <FloatArrayLike T>
    void multiply(T& arr, typename T::value_type scalar) noexcept {
        multiply(arr.data(), scalar, arr.size());
    }

    /**
        Divides the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void divide(T* lhs, const T* rhs, size_t size) noexcept;

    /**
        Divides the values of `lhs` by the value of `scalar`, and
        stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to divide each element in `arr` by.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void divide(T* arr, T scalar, size_t size) noexcept;

    /**
        Divides the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatArrayLike T>
    void divide(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        divide(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Divides the values of `lhs` by the value of `scalar`, and
        stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to divide each element in `arr` by.
    */
    template <FloatArrayLike T>
    void divide(T& arr, typename T::value_type scalar) noexcept {
        divide(arr.data(), scalar, arr.size());
    }

    /**
        Copies the contents of rhs into lhs.
        \param lhs A raw pointer to the destination array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements to copy.
    */
    template <NumericType T>
    void copy(T* lhs, const T* rhs, size_t size) noexcept {
        std::memcpy(lhs, rhs, sizeof(T) * size);
    }


    /**
        Copies the contents of rhs into lhs.
        `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <ArrayLike T>
    void copy(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        copy(lhs.data(), rhs.data(), lhs.size());
    }
} // namespace marvin::vecops
#endif