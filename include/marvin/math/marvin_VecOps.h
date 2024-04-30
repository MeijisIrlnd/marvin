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
#include <mipp.h>
#include <type_traits>
namespace marvin::vecops {

    /**
        Uses SIMD operations to add the values of `rhs` to the values of `lhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <NumericType T>
    void add(T* lhs, const T* rhs, size_t size) noexcept {
        const auto vectorisedLoopSize = (size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> rl, rr;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            rl.load(&lhs[i]);
            rr.load(&rhs[i]);
            rl += rr;
            rl.store(&lhs[i]);
        }
        for (auto i = vectorisedLoopSize; i < size; ++i) {
            lhs[i] += rhs[i];
        }
    }

    /**
        Uses SIMD operations to add the value of `scalar` to the values of `lhs`,
        and stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to add to each element of `arr`.
        \param size The number of elements in `arr`.
    */
    template <NumericType T>
    void add(T* arr, T scalar, size_t size) noexcept {
        const auto vectorisedLoopSize = (size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> r;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            r.load(&arr[i]);
            r += scalar;
            r.store(&arr[i]);
        }
        for (auto i = vectorisedLoopSize; i < size; ++i) {
            arr[i] += scalar;
        }
    }

    /**
        Uses SIMD operations to add the values of `rhs` to the values of `lhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <ArrayLike T>
    requires std::is_floating_point_v<typename T::value_type> ||
             std::is_integral_v<typename T::value_type>
    void add(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        add(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Uses SIMD operations to add the value of `scalar` to the values of `lhs`,
        and stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to add to each element of `arr`.
    */
    template <ArrayLike T>
    requires std::is_floating_point_v<typename T::value_type> ||
             std::is_integral_v<typename T::value_type>
    void add(T& arr, typename T::value_type scalar) noexcept {
        add(arr.data(), scalar, arr.size());
    }

    /**
        Uses SIMD operations to subtract the values of `rhs` from the values of `lhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <NumericType T>
    void subtract(T* lhs, const T* rhs, size_t size) noexcept {
        const auto vectorisedLoopSize = (size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> rl, rr;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            rl.load(&lhs[i]);
            rr.load(&rhs[i]);
            rl -= rr;
            rl.store(&lhs[i]);
        }
        for (auto i = vectorisedLoopSize; i < size; ++i) {
            lhs[i] -= rhs[i];
        }
    }

    /**
        Uses SIMD operations to subtract the value of `scalar` from the values of `lhs`, and
        stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to subtract from each element in `arr`.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <NumericType T>
    void subtract(T* arr, T scalar, size_t size) noexcept {
        const auto vectorisedLoopSize = (size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> r;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            r.load(&arr[i]);
            r -= scalar;
            r.store(&arr[i]);
        }
        for (auto i = vectorisedLoopSize; i < size; ++i) {
            arr[i] -= scalar;
        }
    }

    /**
        Uses SIMD operations to subtract the values of `rhs` from the values of `lhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <ArrayLike T>
    requires std::is_floating_point_v<typename T::value_type> ||
             std::is_integral_v<typename T::value_type>
    void subtract(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        subtract(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Uses SIMD operations to subtract the value of `scalar` from the values of `lhs`, and
        stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to subtract from each element in `arr`.
    */
    template <ArrayLike T>
    requires std::is_floating_point_v<typename T::value_type> ||
             std::is_integral_v<typename T::value_type>
    void subtract(T& arr, typename T::value_type scalar) noexcept {
        subtract(arr.data(), scalar, arr.size());
    }

    /**
        Uses SIMD operations to multiply the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <NumericType T>
    void multiply(T* lhs, const T* rhs, size_t size) noexcept {
        const auto vectorisedLoopSize = (size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> rl, rr;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            rl.load(&lhs[i]);
            rr.load(&rhs[i]);
            rl *= rr;
            rl.store(&lhs[i]);
        }
        for (auto i = vectorisedLoopSize; i < size; ++i) {
            lhs[i] *= rhs[i];
        }
    }

    /**
        Uses SIMD operations to multiply the values of `lhs` by value of `scalar`, and
        stores the result in `lhs`.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to multiply each element in `arr` by.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <NumericType T>
    void multiply(T* arr, T scalar, size_t size) noexcept {
        const auto vectorisedLoopSize = (size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> r;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            r.load(&arr[i]);
            r *= scalar;
            r.store(&arr[i]);
        }
        for (auto i = vectorisedLoopSize; i < size; ++i) {
            arr[i] *= scalar;
        }
    }

    /**
        Uses SIMD operations to multiply the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <ArrayLike T>
    requires std::is_floating_point_v<typename T::value_type> ||
             std::is_integral_v<typename T::value_type>
    void multiply(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        multiply(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Uses SIMD operations to multiply the values of `lhs` by the value of `scalar`, and
        stores the result in `lhs`.
        \param arr The destination array-like.
        \param scalar The value to multiply each element in `arr` by.
    */
    template <ArrayLike T>
    requires std::is_floating_point_v<typename T::value_type> ||
             std::is_integral_v<typename T::value_type>
    void multiply(T& arr, typename T::value_type scalar) noexcept {
        multiply(arr.data(), scalar, arr.size());
    }

    /**
        Uses SIMD operations to divide the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`. Note that because AVX and MSSE4.2 don't support int division ops,
        the constraint on T here is stricter than on other vecops.
        \param lhs A raw pointer to the dest array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void divide(T* lhs, const T* rhs, size_t size) noexcept {
        const auto vectorisedLoopSize = (size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> rl, rr;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            rl.load(&lhs[i]);
            rr.load(&rhs[i]);
            rl /= rr;
            rl.store(&lhs[i]);
        }
        for (auto i = vectorisedLoopSize; i < size; ++i) {
            lhs[i] /= rhs[i];
        }
    }

    /**
        Uses SIMD operations to divide the values of `lhs` by the value of `scalar`, and
        stores the result in `lhs`. Note that because AVX and MSSE4.2 don't support int division ops,
        the constraint on T here is stricter than on other vecops.
        \param arr A raw pointer to the dest array-like.
        \param scalar The value to divide each element in `arr` by.
        \param size The number of elements in `lhs` and `rhs`.
    */
    template <FloatType T>
    void divide(T* arr, T scalar, size_t size) noexcept {
        const auto vectorisedLoopSize = (size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> r;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            r.load(&arr[i]);
            r /= scalar;
            r.store(&arr[i]);
        }
        for (auto i = vectorisedLoopSize; i < size; ++i) {
            arr[i] /= scalar;
        }
    }

    /**
        Uses SIMD operations to divide the values of `lhs` by the values of `rhs`, and
        stores the result in `lhs`. `lhs.size()` <b>must</b> == `rhs.size()`. Note that because AVX and MSSE4.2 don't support int division ops,
        the constraint on T here is stricter than on other vecops.
        \param lhs The destination array-like.
        \param rhs The source array-like.
    */
    template <ArrayLike T>
    requires std::is_floating_point_v<typename T::value_type>
    void divide(T& lhs, const T& rhs) noexcept {
        assert(lhs.size() == rhs.size());
        divide(lhs.data(), rhs.data(), lhs.size());
    }

    /**
        Uses SIMD operations to divide the values of `lhs` by the value of `scalar`, and
        stores the result in `lhs`. Note that because AVX and MSSE4.2 don't support int division ops,
        the constraint on T here is stricter than on other vecops.
        \param arr The destination array-like.
        \param scalar The value to divide each element in `arr` by.
    */
    template <ArrayLike T>
    requires std::is_floating_point_v<typename T::value_type>
    void divide(T& arr, typename T::value_type scalar) noexcept {
        divide(arr.data(), scalar, arr.size());
    }

    /**
        Wrapper around `std::memcpy`.
        \param lhs A raw pointer to the destination array-like.
        \param rhs A raw pointer to the source array-like.
        \param size The number of elements to copy.
    */
    template <typename T>
    void copy(T* lhs, const T* rhs, size_t size) noexcept {
        std::memcpy(lhs, rhs, sizeof(T) * size);
    }

    /**
        Wrapper around memcpy, to make it slightly harder to forget to call `sizeof(T)`.
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