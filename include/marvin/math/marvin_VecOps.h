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
#include <span>
namespace marvin::math::vecops {

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
        Adds the values of `rhs` to the values of `lhs`, and stores
        the result in `dest`.
        \param dest A raw pointer to the dest array-like.
        \param lhs A raw pointer to an array-like.
        \param rhs A raw pointer to an array-like.
        \param size The number of elements in `lhs`, `rhs` and `dest`.
     */
    template <FloatType T>
    void add(T* dest, const T* lhs, const T* rhs, size_t size) noexcept;
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
        Adds the value of `scalar` to the values of `lhs`,
        and stores the result in `dest`.
        \param dest A raw pointer to the dest array-like.
        \param arr A raw pointer to an array-like.
        \param scalar The value to add to each element of `arr`.
        \param size The number of elements in `arr`.
    */
    template <FloatType T>
    void add(T* dest, const T* arr, T scalar, size_t size) noexcept;

    /**
        Adds the values of `rhs` to the values of `lhs` and stores the result in `lhs`.
        \param lhs An array-like.
        \param rhs An array-like.
    */
    template <FloatType T>
    void add(std::span<T> lhs, std::span<T> rhs) noexcept {
        assert(lhs.size() == rhs.size());
        add(lhs.data(), rhs.data(), lhs.size());
    }
    /**
        Adds the values of `rhs` to the values of `lhs`, and
        stores the result in `dest`. `lhs.size()` <b>must</b> == `rhs.size()` and `dest.size()`.
        \param dest The destination array-like.
        \param lhs An array-like.
        \param rhs An array-like.
    */
    template <FloatType T>
    void add(std::span<T> dest, std::span<T> lhs, std::span<T> rhs) noexcept {
        assert(dest.size() == lhs.size());
        assert(lhs.size() == rhs.size());
        add(dest.data(), lhs.data(), rhs.data(), dest.size());
    }

    /**
        Adds the value of `scalar` to the values of `arr`,
        and stores the result in `arr`.
        \param arr The destination array-like.
        \param scalar The value to add to each element of `arr`.
    */
    template <FloatType T>
    void add(std::span<T> arr, T scalar) noexcept {
        add(arr.data(), scalar, arr.size());
    }

    /**
        Adds the value of `scalar` to the values of `arr`, and stores the result in `dest`.
        `dest.size()` <b>must</b == `arr.size()`.
        \param dest The destination array-like.
        \param arr The source array-like.
        \param scalar The value to add to each element of `arr`.
    */
    template <FloatType T>
    void add(std::span<T> dest, std::span<T> arr, T scalar) noexcept {
        assert(dest.size() == arr.size());
        add(dest.data(), arr.data(), scalar, dest.size());
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
        Subtracts the values of `rhs` from the values of `lhs`, and
        stores the result in `dest`.
        \param dest A raw pointer to the dest array-like.
        \param lhs A raw pointer to an array-like.
        \param rhs A raw pointer to an array-like.
        \param size The number of elements in `lhs`, `rhs` and `dest`.
    */
    template <FloatType T>
    void subtract(T* dest, const T* lhs, const T* rhs, size_t size) noexcept;

    /**
        Subtracts the value of `scalar` from the values of `arr`, and
        stores the result in `arr`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to subtract from each element in `arr`.
        \param size The number of elements in `arr`.
    */
    template <FloatType T>
    void subtract(T* arr, T scalar, size_t size) noexcept;

    /**
        Subtracts the value of `scalar` from the values of `arr`, and
        stores the result in `dest`.
        \param dest A raw pointer to the dest array-like.
        \param arr A raw pointer to the source array-like.
        \param scalar The value to subtract from each element in `arr`.
        \param size The number of elements in `dest` and `arr`.
    */
    template <FloatType T>
    void subtract(T* dest, const T* arr, T scalar, size_t size) noexcept;

    /**
        Subtracts the values of `rhs` from the values of `lhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatType T>
    void subtract(std::span<T> lhs, std::span<T> rhs) noexcept {
        assert(lhs.size() == rhs.size());
        subtract(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Subtracts the values of `rhs` from the values of `lhs`, and
        stores the result in `dest`. `lhs.size()` <b>must</b> == `rhs.size()` and `dest.size()`.
        \param dest The destination array-like.
        \param lhs An array-like.
        \param rhs An array-like.
    */
    template <FloatType T>
    void subtract(std::span<T> dest, std::span<T> lhs, std::span<T> rhs) noexcept {
        assert(dest.size() == lhs.size());
        assert(lhs.size() == rhs.size());
        subtract(dest.data(), lhs.data(), rhs.data(), dest.size());
    }

    /**
        Subtracts the value of `scalar` from the values of `lhs`, and
        stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to subtract from each element in `arr`.
    */
    template <FloatType T>
    void subtract(std::span<T> arr, T scalar) noexcept {
        subtract(arr.data(), scalar, arr.size());
    }

    /**
        Subtracts the value of `scalar` from the values of `arr`, and
        stores the result in `dest`.
        `dest.size()` <b>must</b> == `arr.size()`.
        \param dest The destination array-like.
        \param arr The source array-like.
        \param scalar The value to subtract from each element in `arr`.
    */
    template <FloatType T>
    void subtract(std::span<T> dest, std::span<T> arr, T scalar) noexcept {
        assert(dest.size() == arr.size());
        subtract(dest.data(), arr.data(), scalar, dest.size());
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
        Multiplies the values of `lhs` by the values of `rhs`, and
        stores the result in `dest`.
        \param dest A raw pointer to the dest array-like.
        \param lhs A raw pointer to an array-like.
        \param rhs A raw pointer to an array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void multiply(T* dest, const T* lhs, const T* rhs, size_t size) noexcept;

    /**
        Multiplies the values of `arr` by value of `scalar`, and
        stores the result in `arr`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to multiply each element in `arr` by.
        \param size The number of elements in `arr`.
    */
    template <FloatType T>
    void multiply(T* arr, T scalar, size_t size) noexcept;

    /**
        Multiplies the values of `arr` by value of `scalar`, and
        stores the result in `dest`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to multiply each element in `arr` by.
        \param size The number of elements in `dest` and `arr`.
    */
    template <FloatType T>
    void multiply(T* dest, const T* arr, T scalar, size_t size) noexcept;

    /**
        Multiplies the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatType T>
    void multiply(std::span<T> lhs, std::span<T> rhs) noexcept {
        assert(lhs.size() == rhs.size());
        multiply(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Multiplies the values of `lhs` by the values of `rhs`, and
        stores the result in `dest`. `lhs.size()` <b>must</b> == `rhs.size()` and `dest.size()`.
        \param dest The destination array-like
        \param lhs An array-like.
        \param rhs An array-like.
    */
    template <FloatType T>
    void multiply(std::span<T> dest, std::span<T> lhs, std::span<T> rhs) noexcept {
        assert(dest.size() == lhs.size());
        assert(lhs.size() == rhs.size());
        multiply(dest.data(), lhs.data(), rhs.data(), dest.size());
    }

    /**
        Multiplies the values of `arr` by the value of `scalar`, and
        stores the result in `arr`.
        \param arr The destination array-like.
        \param scalar The value to multiply each element in `arr` by.
    */
    template <FloatType T>
    void multiply(std::span<T> arr, T scalar) noexcept {
        multiply(arr.data(), scalar, arr.size());
    }

    /**
        Multiplies the values of `arr` by the value of `scalar`, and
        stores the result in `dest`.
        \param dest The destination array-like.
        \param arr The source array-like.
        \param scalar The value to multiply each element in `arr` by.
    */
    template <FloatType T>
    void multiply(std::span<T> dest, std::span<T> arr, T scalar) noexcept {
        assert(dest.size() == arr.size());
        multiply(dest.data(), arr.data(), scalar, dest.size());
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
        Divides the values of `lhs` by the values of `rhs`, and
        stores the result in `dest`.
        \param dest A raw pointer to the dest array-like.
        \param lhs A raw pointer to an array-like.
        \param rhs A raw pointer to an array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void divide(T* dest, const T* lhs, const T* rhs, size_t size) noexcept;

    /**
        Divides the values of `arr` by the value of `scalar`, and
        stores the result in `arr`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to divide each element in `arr` by.
        \param size The number of elements in `arr`.
    */
    template <FloatType T>
    void divide(T* arr, T scalar, size_t size) noexcept;

    /**
        Divides the values of `arr` by the value of `scalar`, and
        stores the result in `dest`.
        \param dest A raw pointer to the dest array-like.
        \param arr A raw pointer to the source array-like.
        \param scalar The value to divide each element in `arr` by.
        \param size The number of elements in `arr` and `dest`.
    */
    template <FloatType T>
    void divide(T* dest, const T* arr, T scalar, size_t size) noexcept;

    /**
        Divides the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <FloatType T>
    void divide(std::span<T> lhs, std::span<T> rhs) noexcept {
        assert(lhs.size() == rhs.size());
        divide(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Divides the values of `lhs` by the values of `rhs`, and
        stores the result in `dest`. `lhs.size()` <b>must</b> == `rhs.size()` and `dest.size()`.
        \param dest The destination array-like
        \param lhs An array-like.
        \param rhs An array-like.
    */
    template <FloatType T>
    void divide(std::span<T> dest, std::span<T> lhs, std::span<T> rhs) noexcept {
        assert(dest.size() == lhs.size());
        assert(lhs.size() == rhs.size());
        divide(dest.data(), lhs.data(), rhs.data(), dest.size());
    }

    /**
        Divides the values of `arr` by the value of `scalar`, and
        stores the result in `arr`.
        \param arr The destination array-like.
        \param scalar The value to divide each element in `arr` by.
    */
    template <FloatType T>
    void divide(std::span<T> arr, T scalar) noexcept {
        divide(arr.data(), scalar, arr.size());
    }

    /**
        Divides the values of `arr` by the value of `scalar`, and
        stores the result in `dest`.
        \param dest The destination array-like.
        \param arr The source array-like.
        \param scalar The value to divide each element in `arr` by.
    */
    template <FloatType T>
    void divide(std::span<T> dest, std::span<T> arr, T scalar) noexcept {
        assert(dest.size() == arr.size());
        divide(dest.data(), arr.data(), scalar, dest.size());
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
    template <FloatType T>
    void copy(std::span<T> lhs, std::span<T> rhs) noexcept {
        assert(lhs.size() == rhs.size());
        copy(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Computes the sum of all elements in the given array.
        \param arr A raw pointer to the source array-like.
        \param size The number of elements in the source array-like.
        \return The sum of all elements in the given array-like.
    */
    template <FloatType T>
    [[nodiscard]] T sum(const T* arr, size_t size) noexcept;

    /**
        Computes the sum of all elements in the given array.
        \param arr The source array-like.
        \param size The number of elements in the source array-like.
        \return The sum of all elements in the given array-like.
    */
    template <FloatType T>
    [[nodiscard]] T sum(std::span<T> arr) noexcept {
        return sum(arr.data(), arr.size());
    }


} // namespace marvin::math::vecops
#endif