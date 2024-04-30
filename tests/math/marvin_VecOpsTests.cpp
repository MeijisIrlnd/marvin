// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================
#include "marvin/library/marvin_Concepts.h"
#include <marvin/math/marvin_VecOps.h>
#include <fmt/core.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
namespace marvin::testing {

    template <NumericType T>
    [[nodiscard]] constexpr std::string getTypeName() {
        if constexpr (std::is_same_v<T, float>) {
            return "float";
        } else if constexpr (std::is_same_v<T, double>) {
            return "double";
        } else if constexpr (std::is_same_v<T, int>) {
            return "int";
        }
    }

    template <NumericType T, size_t N>
    void testVecVecAdd() {
        const auto typeStr{ getTypeName<T>() };
        std::vector<T> lhs, rhs;
        for (auto i = 0; i < N; ++i) {
            lhs.emplace_back(i);
            rhs.emplace_back(i);
        }
        std::span<T, N> lhsView{ lhs.data(), N };
        std::span<T, N> rhsView{ rhs.data(), N };
        vecops::add(lhsView, rhsView);
        for (auto i = 0; i < N; ++i) {
            REQUIRE(lhs[i] == static_cast<T>(2.0) * rhs[i]);
        }
        std::memcpy(lhs.data(), rhs.data(), sizeof(T) * N);
        BENCHMARK(fmt::format("Addition fallback<{}>, N = {}", typeStr, N)) {
            for (auto i = 0; i < N; ++i) {
                lhs[i] += rhs[i];
            }
        };
        std::memcpy(lhs.data(), rhs.data(), sizeof(T) * N);
        BENCHMARK(fmt::format("Addition SIMD<{}>, N = {}", typeStr, N)) {
            vecops::add(lhsView, rhsView);
        };
    }

    template <NumericType T, size_t N>
    void testVecScalarAdd() {
        constexpr auto typeStr{ getTypeName<T>() };
        std::vector<T> arr;
        arr.resize(N);
        std::fill(arr.begin(), arr.end(), static_cast<T>(2.0));
        std::span<T, N> view{ arr.data(), N };
        vecops::add(view, static_cast<T>(2.0));
        for (size_t i = 0; i < N; ++i) {
            REQUIRE(arr[i] == static_cast<T>(4.0));
        }
        std::fill(arr.begin(), arr.end(), 2.0);
        BENCHMARK(fmt::format("Add fallback<{}>, Vec + Scalar, N = {}", typeStr, N)) {
            for (size_t i = 0; i < N; ++i) {
                arr[i] += static_cast<T>(2.0);
            }
        };
        std::fill(arr.begin(), arr.end(), 2.0);
        BENCHMARK(fmt::format("Add SIMD<{}>, Vec + Scalar, N = {}", typeStr, N)) {
            vecops::add(view, static_cast<T>(2.0));
        };
    }

    template <NumericType T, size_t N>
    void testSubtract() {
        const auto typeStr{ getTypeName<T>() };
        std::vector<T> lhs, rhs;
        for (auto i = 0; i < N; ++i) {
            lhs.emplace_back(i);
            rhs.emplace_back(i);
        }
        std::span<T, N> lhsView{ lhs.data(), N };
        std::span<T, N> rhsView{ rhs.data(), N };
        vecops::subtract(lhsView, rhsView);
        for (auto i = 0; i < N; ++i) {
            REQUIRE(lhs[i] == 0);
        }
        std::memcpy(lhs.data(), rhs.data(), sizeof(T) * N);
        BENCHMARK(fmt::format("Subtraction fallback<{}>, N = {}", typeStr, N)) {
            for (auto i = 0; i < N; ++i) {
                lhs[i] -= rhs[i];
            }
        };
        std::memcpy(lhs.data(), rhs.data(), sizeof(T) * N);
        BENCHMARK(fmt::format("Subtraction SIMD<{}>, N = {}", typeStr, N)) {
            vecops::subtract(lhsView, rhsView);
        };
    }

    template <NumericType T, size_t N>
    void testVecVecMultiply() {
        const auto typeStr{ getTypeName<T>() };
        std::vector<T> lhs, rhs;
        for (auto i = 0; i < N; ++i) {
            lhs.emplace_back(i);
            rhs.emplace_back(i);
        }
        vecops::multiply<T, N>(std::span<T, N>{ lhs.data(), N }, std::span<T, N>{ rhs.data(), N });
        for (auto i = 0; i < N; ++i) {
            REQUIRE(lhs[i] == std::pow(rhs[i], 2));
        }
        std::memcpy(lhs.data(), rhs.data(), sizeof(T) * N);
        std::span<T, N> lhsView{ lhs.data(), N };
        std::span<T, N> rhsView{ rhs.data(), N };

        BENCHMARK(fmt::format("Multiply fallback<{}>, Vec * Vec, N = {}", typeStr, N)) {
            for (auto i = 0; i < N; ++i) {
                lhs[i] *= rhs[i];
            }
        };
        std::memcpy(lhs.data(), rhs.data(), sizeof(T) * N);
        BENCHMARK(fmt::format("Multiply SIMD<{}>, Vec * Vec, N = {}", typeStr, N)) {
            vecops::multiply(lhsView, rhsView);
        };
    }

    template <NumericType T, size_t N>
    void testVecScalarMultiply() {
        constexpr auto typeStr{ getTypeName<T>() };
        std::vector<T> arr;
        for (auto i = 0; i < N; ++i) {
            arr.emplace_back(static_cast<T>(i));
        }
        std::span<T, N> view{ arr.data(), N };
        vecops::multiply(view, static_cast<T>(2.0));
        for (auto i = 0; i < N; ++i) {
            REQUIRE(arr[i] == static_cast<T>(i) * static_cast<T>(2.0));
        }

        for (size_t i = 0; i < N; ++i) {
            arr[i] = static_cast<T>(i);
        }
        BENCHMARK(fmt::format("Multiply fallback<{}>, Vec * Scalar, N = {}", typeStr, N)) {
            for (size_t i = 0; i < N; ++i) {
                arr[i] *= static_cast<T>(2.0);
            }
        };
        for (size_t i = 0; i < N; ++i) {
            arr[i] = static_cast<T>(i);
        }
        BENCHMARK(fmt::format("Multiply SIMD<{}>, Vec * Scalar, N = {}", typeStr, N)) {
            vecops::multiply(view, static_cast<T>(2.0));
        };
    }

    template <FloatType T, size_t N>
    void testDivide() {
        const auto typeStr{ getTypeName<T>() };
        std::vector<T> lhs, rhs;
        for (auto i = 1; i < N + 1; ++i) {
            lhs.emplace_back(i);
            rhs.emplace_back(i);
        }
        vecops::divide<T, N>(std::span<T, N>{ lhs.data(), N }, std::span<T, N>{ rhs.data(), N });
        for (auto i = 0; i < N; ++i) {
            REQUIRE(lhs[i] == static_cast<T>(1.0));
        }
        std::memcpy(lhs.data(), rhs.data(), sizeof(T) * N);
        std::span<T, N> lhsView{ lhs.data(), N };
        std::span<T, N> rhsView{ rhs.data(), N };

        BENCHMARK(fmt::format("Divide fallback<{}>, N = {}", typeStr, N)) {
            for (auto i = 0; i < N; ++i) {
                lhs[i] /= rhs[i];
            }
        };
        std::memcpy(lhs.data(), rhs.data(), sizeof(T) * N);
        BENCHMARK(fmt::format("Divide SIMD<{}>, N = {}", typeStr, N)) {
            vecops::divide(lhsView, rhsView);
        };
    }

    template <NumericType T, size_t N>
    void testCopy() {
        const auto typeStr{ getTypeName<T>() };
        std::vector<T> lhs, rhs;
        for (auto i = 0; i < N; ++i) {
            lhs.emplace_back(static_cast<T>(-1.0));
            rhs.emplace_back(i);
        }
        std::span<T, N> lhsView{ lhs.data(), N };
        std::span<T, N> rhsView{ rhs.data(), N };
        vecops::copy(lhsView, rhsView);
        for (auto i = 0; i < N; ++i) {
            REQUIRE(lhs[i] == rhs[i]);
        }
    }

    TEST_CASE("Test VecOps") {

        SECTION("Test Add") {
            testVecVecAdd<int, 8>();
            testVecVecAdd<float, 8>();
            testVecVecAdd<double, 8>();
            testVecVecAdd<int, 16>();
            testVecVecAdd<float, 16>();
            testVecVecAdd<double, 16>();
            testVecVecAdd<int, 32>();
            testVecVecAdd<float, 32>();
            testVecVecAdd<double, 32>();
            testVecScalarAdd<int, 8>();
            testVecScalarAdd<float, 8>();
            testVecScalarAdd<double, 8>();
            testVecScalarAdd<int, 16>();
            testVecScalarAdd<float, 16>();
            testVecScalarAdd<double, 16>();
            testVecScalarAdd<int, 32>();
            testVecScalarAdd<float, 32>();
            testVecScalarAdd<double, 32>();
        }

        SECTION("Test Subtract") {
            testSubtract<int, 8>();
            testSubtract<float, 8>();
            testSubtract<double, 8>();
            testSubtract<int, 16>();
            testSubtract<float, 16>();
            testSubtract<double, 16>();
            testSubtract<int, 32>();
            testSubtract<float, 32>();
            testSubtract<double, 32>();
        }


        SECTION("Test Multiply") {
            testVecVecMultiply<int, 8>();
            testVecVecMultiply<float, 8>();
            testVecVecMultiply<double, 8>();
            testVecVecMultiply<int, 16>();
            testVecVecMultiply<float, 16>();
            testVecVecMultiply<double, 16>();
            testVecVecMultiply<int, 32>();
            testVecVecMultiply<float, 32>();
            testVecVecMultiply<double, 32>();
            testVecScalarMultiply<int, 8>();
            testVecScalarMultiply<float, 8>();
            testVecScalarMultiply<double, 8>();
            testVecScalarMultiply<int, 16>();
            testVecScalarMultiply<float, 16>();
            testVecScalarMultiply<double, 16>();
            testVecScalarMultiply<int, 32>();
            testVecScalarMultiply<float, 32>();
            testVecScalarMultiply<double, 32>();
        }

        SECTION("Test Divide") {
            testDivide<float, 8>();
            testDivide<double, 8>();
            testDivide<float, 16>();
            testDivide<double, 16>();
            testDivide<float, 32>();
            testDivide<double, 32>();
        }
    }


} // namespace marvin::testing
