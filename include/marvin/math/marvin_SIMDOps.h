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
namespace marvin::simd {

    /**
        Uses SIMD operations to add the values of the rhs array-like to the lhs array-like,
        and stores the result in lhs.
        \param lhs The destination array-like, to add the values to.
        \param rhs The array-like to add the values from.
    */
    template <NumericType T, size_t Size>
    void add(std::span<T, Size> lhs, std::span<T, Size> rhs) noexcept {
        const auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
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
        Uses SIMD operations to subtract the values of the rhs array-like from the values of the lhs array-like,
        and stores the result in lhs.
        \param lhs The destination array-like, to subtract the values from.
        \param rhs The array-like containing the values to subtract.
    */
    template <NumericType T, size_t Size>
    void subtract(std::span<T, Size> lhs, std::span<T, Size> rhs) noexcept {
        const auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
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
        Uses SIMD operations to multiply the values of the lhs array-like with the values of the rhs array like,
        and stores the result in lhs.
        \param lhs The destination array-like.
        \param rhs The array-like to multiply lhs by.
    */
    template <NumericType T, size_t Size>
    void multiply(std::span<T, Size> lhs, std::span<T, Size> rhs) noexcept {
        const auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
        mipp::Reg<T> rl, rr, rtemp;
        for (auto i = 0_sz; i < vectorisedLoopSize; i += mipp::N<T>()) {
            rl.load(&lhs[i]);
            rr.load(&rhs[i]);
            rtemp = rl * rr;
            rtemp.store(&lhs[i]);
        }
        for (auto i = vectorisedLoopSize; i < Size; ++i) {
            lhs[i] *= rhs[i];
        }
    }

    /**
        Uses SIMD operations to divide the values in the lhs array-like by the values in the rhs array-like,
        and stores the result in lhs.
        \param lhs The destination array-like.
        \param rhs The array-like to divide lhs by.
    */
    template <NumericType T, size_t Size>
    void divide(std::span<T, Size> lhs, std::span<T, Size> rhs) noexcept {
        const auto vectorisedLoopSize = (Size / mipp::N<T>()) * mipp::N<T>();
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

} // namespace marvin::simd
#endif