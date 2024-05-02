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
#include <array>
#include <span>
#include <vector>
#include <cstring>
namespace marvin::testing {

    template <NumericType T>
    [[nodiscard]] std::string getTypeName() {
        if constexpr (std::is_same_v<T, float>) {
            return "float";
        } else if constexpr (std::is_same_v<T, double>) {
            return "double";
        } else if constexpr (std::is_same_v<T, int>) {
            return "int";
        }
    }

    template <NumericType T, size_t N>
    void testAdd() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(2.0));
            marvin::vecops::add(lhsArr, rhsArr);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(2.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::vecops::add(lhsArr, static_cast<T>(5.0));
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(5.0));
            }
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> lhsVec, rhsVec;
            lhsVec.resize(N);
            rhsVec.resize(N);
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(0.0));
            std::fill(rhsVec.begin(), rhsVec.end(), static_cast<T>(-1.0));
            marvin::vecops::add(lhsVec, rhsVec);
            for (const auto& el : lhsVec) {
                REQUIRE(el == static_cast<T>(-1.0));
            }
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(0.0));
            marvin::vecops::add(lhsVec, static_cast<T>(10.0));
            for (const auto& el : lhsVec) {
                REQUIRE(el == static_cast<T>(10.0));
            }
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(-10.0));
            std::span<T, N> lhsView{ lhsArr.data(), N };
            std::span<T, N> rhsView{ rhsArr.data(), N };
            marvin::vecops::add(lhsView, rhsView);
            for (const auto& el : lhsView) {
                REQUIRE(el == static_cast<T>(-10.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::vecops::add(lhsView, static_cast<T>(100.0));
            for (const auto& el : lhsView) {
                REQUIRE(el == static_cast<T>(100.0));
            }
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(30.0));
            marvin::vecops::add(lhsArr.data(), rhsArr.data(), N);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(30.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::vecops::add(lhsArr.data(), static_cast<T>(15.0), N);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(15.0));
            }
        }
    }

    template <NumericType T, size_t N>
    void benchmarkAdd() {
        const auto typeName = getTypeName<T>();
        std::array<T, N> lhs, rhs;
        std::fill(lhs.begin(), lhs.end(), static_cast<T>(0.0));
        std::fill(rhs.begin(), rhs.end(), static_cast<T>(100.0));
        BENCHMARK(fmt::format("Add fallback: std::array<{}, {}>", typeName, N)) {
            for (size_t i = 0; i < N; ++i) {
                lhs[i] += rhs[i];
            }
        };
        std::fill(lhs.begin(), lhs.end(), static_cast<T>(0.0));
        BENCHMARK(fmt::format("Add SIMD: std::array<{}, {}>", typeName, N)) {
            vecops::add(lhs, rhs);
        };
    }
    template <NumericType T, size_t N>
    void testSubtract() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(2.0));
            marvin::vecops::subtract(lhsArr, rhsArr);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(-2.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::vecops::subtract(lhsArr, static_cast<T>(5.0));
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(-5.0));
            }
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> lhsVec, rhsVec;
            lhsVec.resize(N);
            rhsVec.resize(N);
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(0.0));
            std::fill(rhsVec.begin(), rhsVec.end(), static_cast<T>(-1.0));
            marvin::vecops::subtract(lhsVec, rhsVec);
            for (const auto& el : lhsVec) {
                REQUIRE(el == static_cast<T>(1.0));
            }
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(0.0));
            marvin::vecops::subtract(lhsVec, static_cast<T>(10.0));
            for (const auto& el : lhsVec) {
                REQUIRE(el == static_cast<T>(-10.0));
            }
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(-10.0));
            std::span<T, N> lhsView{ lhsArr.data(), N };
            std::span<T, N> rhsView{ rhsArr.data(), N };
            marvin::vecops::add(lhsView, rhsView);
            for (const auto& el : lhsView) {
                REQUIRE(el == static_cast<T>(-10.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::vecops::subtract(lhsView, static_cast<T>(100.0));
            for (const auto& el : lhsView) {
                REQUIRE(el == static_cast<T>(-100.0));
            }
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(30.0));
            marvin::vecops::subtract(lhsArr.data(), rhsArr.data(), N);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(-30.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::vecops::subtract(lhsArr.data(), static_cast<T>(15.0), N);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(-15.0));
            }
        }
    }

    template <NumericType T, size_t N>
    void benchmarkSubtract() {
        const auto typeName = getTypeName<T>();
        std::array<T, N> lhs, rhs;
        std::fill(lhs.begin(), lhs.end(), static_cast<T>(0.0));
        std::fill(rhs.begin(), rhs.end(), static_cast<T>(100.0));
        BENCHMARK(fmt::format("Subtract fallback: std::array<{}, {}>", typeName, N)) {
            for (size_t i = 0; i < N; ++i) {
                lhs[i] -= rhs[i];
            }
        };
        std::fill(lhs.begin(), lhs.end(), static_cast<T>(0.0));
        BENCHMARK(fmt::format("Subtract SIMD: std::array<{}, {}>", typeName, N)) {
            vecops::subtract(lhs, rhs);
        };
    }

    template <NumericType T, size_t N>
    void testMultiply() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(2.0));
            marvin::vecops::multiply(lhsArr, rhsArr);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(4.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            marvin::vecops::multiply(lhsArr, static_cast<T>(5.0));
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(10.0));
            }
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> lhsVec, rhsVec;
            lhsVec.resize(N);
            rhsVec.resize(N);
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(2.0));
            std::fill(rhsVec.begin(), rhsVec.end(), static_cast<T>(-1.0));
            marvin::vecops::multiply(lhsVec, rhsVec);
            for (const auto& el : lhsVec) {
                REQUIRE(el == static_cast<T>(-2.0));
            }
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(2.0));
            marvin::vecops::multiply(lhsVec, static_cast<T>(10.0));
            for (const auto& el : lhsVec) {
                REQUIRE(el == static_cast<T>(20.0));
            }
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(-10.0));
            std::span<T, N> lhsView{ lhsArr.data(), N };
            std::span<T, N> rhsView{ rhsArr.data(), N };
            marvin::vecops::multiply(lhsView, rhsView);
            for (const auto& el : lhsView) {
                REQUIRE(el == static_cast<T>(-20.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            marvin::vecops::multiply(lhsView, static_cast<T>(100.0));
            for (const auto& el : lhsView) {
                REQUIRE(el == static_cast<T>(200.0));
            }
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(30.0));
            marvin::vecops::multiply(lhsArr.data(), rhsArr.data(), N);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(60.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            marvin::vecops::multiply(lhsArr.data(), static_cast<T>(15.0), N);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(30.0));
            }
        }
    }

    template <NumericType T, size_t N>
    void benchmarkMultiply() {
        const auto typeName = getTypeName<T>();
        std::array<T, N> lhs, rhs;
        std::fill(lhs.begin(), lhs.end(), static_cast<T>(2.0));
        std::fill(rhs.begin(), rhs.end(), static_cast<T>(100.0));
        BENCHMARK(fmt::format("Multiply fallback: std::array<{}, {}>", typeName, N)) {
            for (size_t i = 0; i < N; ++i) {
                lhs[i] *= rhs[i];
            }
        };
        std::fill(lhs.begin(), lhs.end(), static_cast<T>(2.0));
        BENCHMARK(fmt::format("Multiply SIMD: std::array<{}, {}>", typeName, N)) {
            vecops::subtract(lhs, rhs);
        };
    }
    template <FloatType T, size_t N>
    void testDivide() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(8.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(2.0));
            marvin::vecops::divide(lhsArr, rhsArr);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(4.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(15.0));
            marvin::vecops::divide(lhsArr, static_cast<T>(5.0));
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(3.0));
            }
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> lhsVec, rhsVec;
            lhsVec.resize(N);
            rhsVec.resize(N);
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(2.0));
            std::fill(rhsVec.begin(), rhsVec.end(), static_cast<T>(1.0));
            marvin::vecops::divide(lhsVec, rhsVec);
            for (const auto& el : lhsVec) {
                REQUIRE(el == static_cast<T>(2.0));
            }
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(10.0));
            marvin::vecops::divide(lhsVec, static_cast<T>(2.5));
            for (const auto& el : lhsVec) {
                REQUIRE(el == static_cast<T>(4.0));
            }
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(32.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(16.0));
            std::span<T, N> lhsView{ lhsArr.data(), N };
            std::span<T, N> rhsView{ rhsArr.data(), N };
            marvin::vecops::divide(lhsView, rhsView);
            for (const auto& el : lhsView) {
                REQUIRE(el == static_cast<T>(2.0));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(1000.0));
            marvin::vecops::divide(lhsView, static_cast<T>(100.0));
            for (const auto& el : lhsView) {
                REQUIRE(el == static_cast<T>(10.0));
            }
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(1.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(10.0));
            marvin::vecops::divide(lhsArr.data(), rhsArr.data(), N);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(0.1));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            marvin::vecops::divide(lhsArr.data(), static_cast<T>(4.0), N);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(0.5));
            }
        }
    }

    template <NumericType T, size_t N>
    void benchmarkDivide() {
        const auto typeName = getTypeName<T>();
        std::array<T, N> lhs, rhs;
        std::fill(lhs.begin(), lhs.end(), static_cast<T>(2.0));
        std::fill(rhs.begin(), rhs.end(), static_cast<T>(100.0));
        BENCHMARK(fmt::format("Divide fallback: std::array<{}, {}>", typeName, N)) {
            for (size_t i = 0; i < N; ++i) {
                lhs[i] /= rhs[i];
            }
        };
        std::fill(lhs.begin(), lhs.end(), static_cast<T>(2.0));
        BENCHMARK(fmt::format("Divide SIMD: std::array<{}, {}>", typeName, N)) {
            vecops::subtract(lhs, rhs);
        };
    }

    template <NumericType T, size_t N>
    void testCopy() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(2.0));
            marvin::vecops::copy(lhsArr, rhsArr);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(2.0));
            }
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> lhsVec, rhsVec;
            lhsVec.resize(N);
            rhsVec.resize(N);
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(2.0));
            std::fill(rhsVec.begin(), rhsVec.end(), static_cast<T>(1.0));
            marvin::vecops::copy(lhsVec, rhsVec);
            for (const auto& el : lhsVec) {
                REQUIRE(el == static_cast<T>(1.0));
            }
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(32.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(16.0));
            std::span<T, N> lhsView{ lhsArr.data(), N };
            std::span<T, N> rhsView{ rhsArr.data(), N };
            marvin::vecops::copy(lhsView, rhsView);
            for (const auto& el : lhsView) {
                REQUIRE(el == static_cast<T>(16.0));
            }
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(1.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(10.0));
            marvin::vecops::copy(lhsArr.data(), rhsArr.data(), N);
            for (const auto& el : lhsArr) {
                REQUIRE(el == static_cast<T>(10.0));
            }
        }
    }


    TEST_CASE("Test VecOps") {

        SECTION("Test Add") {
            testAdd<float, 4>();
            testAdd<double, 4>();
            testAdd<int, 4>();
            testAdd<float, 5>();
            testAdd<double, 5>();
            testAdd<int, 5>();
            testAdd<float, 8>();
            testAdd<double, 8>();
            testAdd<int, 8>();
            testAdd<float, 9>();
            testAdd<double, 9>();
            testAdd<int, 9>();
            testAdd<float, 16>();
            testAdd<double, 16>();
            testAdd<int, 16>();
            testAdd<float, 17>();
            testAdd<double, 17>();
            testAdd<int, 17>();
        }
#if defined(MARVIN_BENCHMARK)
        SECTION("Benchmark Add") {
            benchmarkAdd<float, 32>();
            benchmarkAdd<double, 32>();
            benchmarkAdd<int, 32>();
        }
#endif
        SECTION("Test Subtract") {
            testSubtract<float, 4>();
            testSubtract<double, 4>();
            testSubtract<int, 4>();
            testSubtract<float, 5>();
            testSubtract<double, 5>();
            testSubtract<int, 5>();
            testSubtract<float, 8>();
            testSubtract<double, 8>();
            testSubtract<int, 8>();
            testSubtract<float, 9>();
            testSubtract<double, 9>();
            testSubtract<int, 9>();
            testSubtract<float, 16>();
            testSubtract<double, 16>();
            testSubtract<int, 16>();
            testSubtract<float, 17>();
            testSubtract<double, 17>();
            testSubtract<int, 17>();
        }
#if defined(MARVIN_BENCHMARK)
        SECTION("Benchmark Subtract") {
            benchmarkSubtract<float, 32>();
            benchmarkSubtract<double, 32>();
            benchmarkSubtract<int, 32>();
        }
#endif
        SECTION("Test Multiply") {
            testMultiply<float, 4>();
            testMultiply<double, 4>();
            testMultiply<int, 4>();
            testMultiply<float, 5>();
            testMultiply<double, 5>();
            testMultiply<int, 5>();
            testMultiply<float, 8>();
            testMultiply<double, 8>();
            testMultiply<int, 8>();
            testMultiply<float, 9>();
            testMultiply<double, 9>();
            testMultiply<int, 9>();
            testMultiply<float, 16>();
            testMultiply<double, 16>();
            testMultiply<int, 16>();
            testMultiply<float, 17>();
            testMultiply<double, 17>();
            testMultiply<int, 17>();
        }
#if defined(MARVIN_BENCHMARK)
        SECTION("Benchmark Multiply") {
            benchmarkMultiply<float, 32>();
            benchmarkMultiply<double, 32>();
            benchmarkMultiply<int, 32>();
        }
#endif
        SECTION("Test Divide") {
            testDivide<float, 4>();
            testDivide<double, 4>();
            testDivide<float, 5>();
            testDivide<double, 5>();
            testDivide<float, 8>();
            testDivide<double, 8>();
            testDivide<float, 9>();
            testDivide<double, 9>();
            testDivide<float, 16>();
            testDivide<double, 16>();
            testDivide<float, 17>();
            testDivide<double, 17>();
        }
#if defined(MARVIN_BENCHMARK)
        SECTION("Benchmark Divide") {
            benchmarkDivide<float, 32>();
            benchmarkDivide<double, 32>();
            benchmarkDivide<int, 32>();
        }
#endif
        SECTION("Test Copy") {
            testCopy<float, 4>();
            testCopy<double, 4>();
            testCopy<float, 5>();
            testCopy<double, 5>();
            testCopy<float, 8>();
            testCopy<double, 8>();
            testCopy<float, 9>();
            testCopy<double, 9>();
            testCopy<float, 16>();
            testCopy<double, 16>();
            testCopy<float, 17>();
            testCopy<double, 17>();
        }
    }
} // namespace marvin::testing
