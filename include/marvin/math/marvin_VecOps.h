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
#include <span>
namespace marvin::vecops {

    /**
        Uses SIMD operations to add the values of the rhs view to the lhs view,
        and stores the result in lhs.
        \param lhs The destination view, to add the values to.
        \param rhs The view to add the values from.
    */
    template <NumericType T, size_t Size>
    void add(std::span<T, Size> lhs, std::span<T, Size> rhs) noexcept {
        constexpr auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> rl, rr, rtemp;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            rl.load(&lhs[i]);
            rr.load(&rhs[i]);
            rtemp = rl + rr;
            rtemp.store(&lhs[i]);
        }
        for (auto i = vectorisedLoopSize; i < Size; ++i) {
            lhs[i] += rhs[i];
        }
    }

    /**
        Uses SIMD operations to add the value of `scalar` to the values of `arrView`, and stores the result in `arrView`.
        \param arrView The destination view.
        \param scalar A scalar to add to every element in the view.
    */
    template <NumericType T, size_t Size>
    void add(std::span<T, Size> arrView, T scalar) noexcept {
        constexpr auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> r;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            r.load(&arrView[i]);
            r += scalar;
            r.store(&arrView[i]);
        }
        for (auto i = vectorisedLoopSize; i < Size; ++i) {
            arrView[i] += scalar;
        }
    }

    /**
        Uses SIMD operations to subtract the values of the rhs view from the values of the lhs view,
        and stores the result in lhs.
        \param lhs The destination view, to subtract the values from.
        \param rhs The view containing the values to subtract.
    */
    template <NumericType T, size_t Size>
    void subtract(std::span<T, Size> lhs, std::span<T, Size> rhs) noexcept {
        constexpr auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> rl, rr, rtemp;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            rl.load(&lhs[i]);
            rr.load(&rhs[i]);
            rtemp = rl - rr;
            rtemp.store(&lhs[i]);
        }
        for (auto i = vectorisedLoopSize; i < Size; ++i) {
            lhs[i] -= rhs[i];
        }
    }

    /**
        Uses SIMD operations to subtract the value of `scalar` from the values of `arrView`, and stores the result in `arrView`.
        \param arrView The destination view.
        \param scalar A scalar to subtract from every element in the view.
    */
    template <NumericType T, size_t Size>
    void subtract(std::span<T, Size> arrView, T scalar) noexcept {
        constexpr auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> r;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            r.load(&arrView[i]);
            r -= scalar;
            r.store(&arrView[i]);
        }
        for (auto i = vectorisedLoopSize; i < Size; ++i) {
            arrView[i] += scalar;
        }
    }

    /**
        Uses SIMD operations to multiply the values of the lhs view with the values of the rhs array like,
        and stores the result in lhs.
        \param lhs The destination view.
        \param rhs The view to multiply lhs by.
    */
    template <NumericType T, size_t Size>
    void multiply(std::span<T, Size> lhs, std::span<T, Size> rhs) noexcept {
        const auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
        [[maybe_unused]] const auto n = mipp::N<T>();
        mipp::Reg<T> rl, rr, rtemp;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            rl.load(&lhs[i]);
            rr.load(&rhs[i]);
            rl *= rr;
            rl.store(&lhs[i]);
        }
        for (auto i = vectorisedLoopSize; i < Size; ++i) {
            lhs[i] *= rhs[i];
        }
    }

    /**
        Uses SIMD operations to multiply the values of `arrView` by `scalar`, and stores the result in `arrView`.
        \param arrView The destination view.
        \param scalar A scalar to multiply every element in the view by.
    */
    template <NumericType T, size_t Size>
    void multiply(std::span<T, Size> arrView, T scalar) noexcept {
        constexpr auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> r;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            r.load(&arrView[i]);
            r *= scalar;
            r.store(&arrView[i]);
        }
        for (auto i = vectorisedLoopSize; i < Size; ++i) {
            arrView[i] *= scalar;
        }
    }

    /**
        Uses SIMD operations to divide the values in the lhs view by the values in the rhs view,
        and stores the result in lhs. Note that as avx/sse don't support integer divison, the constraint on T here is stricter
        than in the other SIMD ops.
        \param lhs The destination view.
        \param rhs The view to divide lhs by.
    */
    template <FloatType T, size_t Size>
    void divide(std::span<T, Size> lhs, std::span<T, Size> rhs) noexcept {
        constexpr auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> rl, rr, rtemp;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            rl.load(&lhs[i]);
            rr.load(&rhs[i]);
            rtemp = rl / rr;
            rtemp.store(&lhs[i]);
        }
        for (auto i = vectorisedLoopSize; i < Size; ++i) {
            lhs[i] /= rhs[i];
        }
    }

    /**
        Uses SIMD operations to divide the values of `arrView` by `scalar`, and stores the result in `arrView`.
        \param arrView The destination view.
        \param scalar A scalar to divide every element in the view by.
    */
    template <FloatType T, size_t Size>
    void divide(std::span<T, Size> arrView, T scalar) noexcept {
        constexpr auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> r;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            r.load(&arrView[i]);
            r /= scalar;
            r.store(&arrView[i]);
        }
        for (auto i = vectorisedLoopSize; i < Size; ++i) {
            arrView[i] /= scalar;
        }
    }

    /**
        Literally just a wrapper around `std::memcpy`, to make it slightly harder to forget to use `sizeof`.
        \param lhs The destination to copy the values to.
        \param rhs The source to copy the values from.
    */
    template <FloatType T, size_t Size>
    void copy(std::span<T, Size> lhs, std::span<T, Size> rhs) noexcept {
        std::memcpy(lhs.data(), rhs.data(), sizeof(T) * Size);
    }
} // namespace marvin::vecops
#endif