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
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <array>
#include <span>
#include <vector>
namespace marvin::testing {

    template <FloatType T>
    [[nodiscard]] std::string getTypeName() {
        if constexpr (std::is_same_v<T, float>) {
            return "float";
        } else if constexpr (std::is_same_v<T, double>) {
            return "double";
        } else if constexpr (std::is_same_v<T, int>) {
            return "int";
        }
    }

    template <FloatType T, size_t N>
    void testAdd() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> oopArr;
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(2.0));
            marvin::math::vecops::add<T>(oopArr, lhsArr, rhsArr);
            marvin::math::vecops::add<T>(lhsArr, rhsArr);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(2.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::math::vecops::add<T>(oopArr, lhsArr, static_cast<T>(5.0));
            marvin::math::vecops::add<T>(lhsArr, static_cast<T>(5.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(5.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> oopVec, lhsVec, rhsVec;
            oopVec.resize(N);
            lhsVec.resize(N);
            rhsVec.resize(N);
            std::fill(oopVec.begin(), oopVec.end(), static_cast<T>(0.0));
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(0.0));
            std::fill(rhsVec.begin(), rhsVec.end(), static_cast<T>(-1.0));
            marvin::math::vecops::add<T>(oopVec, lhsVec, rhsVec);
            marvin::math::vecops::add<T>(lhsVec, rhsVec);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsVec[i], Catch::Matchers::WithinRel(static_cast<T>(-1.0f)));
                REQUIRE_THAT(oopVec[i], Catch::Matchers::WithinRel(lhsVec[i]));
            }
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(0.0));
            marvin::math::vecops::add<T>(oopVec, lhsVec, static_cast<T>(10.0));
            marvin::math::vecops::add<T>(lhsVec, static_cast<T>(10.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsVec[i], Catch::Matchers::WithinRel(static_cast<T>(10.0)));
                REQUIRE_THAT(oopVec[i], Catch::Matchers::WithinRel(lhsVec[i]));
            }
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(-10.0));
            std::span<T, N> oopView{ oopArr.data(), N };
            std::span<T, N> lhsView{ lhsArr.data(), N };
            std::span<T, N> rhsView{ rhsArr.data(), N };
            marvin::math::vecops::add<T>(oopView, lhsView, rhsView);
            marvin::math::vecops::add<T>(lhsView, rhsView);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsView[i], Catch::Matchers::WithinRel(static_cast<T>(-10.0)));
                REQUIRE_THAT(oopView[i], Catch::Matchers::WithinRel(lhsView[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::math::vecops::add<T>(oopView, lhsView, static_cast<T>(100.0));
            marvin::math::vecops::add<T>(lhsView, static_cast<T>(100.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsView[i], Catch::Matchers::WithinRel(static_cast<T>(100.0)));
                REQUIRE_THAT(oopView[i], Catch::Matchers::WithinRel(lhsView[i]));
            }
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(30.0));
            marvin::math::vecops::add<T>(oopArr.data(), lhsArr.data(), rhsArr.data(), N);
            marvin::math::vecops::add(lhsArr.data(), rhsArr.data(), N);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(30.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::math::vecops::add<T>(oopArr.data(), lhsArr.data(), static_cast<T>(15.0), N);
            marvin::math::vecops::add(lhsArr.data(), static_cast<T>(15.0), N);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(15.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
        }
    }

    template <FloatType T, size_t N>
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
            math::vecops::add<T>(lhs, rhs);
        };
    }
    template <FloatType T, size_t N>
    void testSubtract() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(2.0));
            marvin::math::vecops::subtract<T>(oopArr, lhsArr, rhsArr);
            marvin::math::vecops::subtract<T>(lhsArr, rhsArr);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(-2.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::math::vecops::subtract<T>(oopArr, lhsArr, static_cast<T>(5.0));
            marvin::math::vecops::subtract<T>(lhsArr, static_cast<T>(5.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(-5.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> oopVec, lhsVec, rhsVec;
            oopVec.resize(N);
            lhsVec.resize(N);
            rhsVec.resize(N);
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(0.0));
            std::fill(rhsVec.begin(), rhsVec.end(), static_cast<T>(-1.0));
            marvin::math::vecops::subtract<T>(oopVec, lhsVec, rhsVec);
            marvin::math::vecops::subtract<T>(lhsVec, rhsVec);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsVec[i], Catch::Matchers::WithinRel(static_cast<T>(1.0)));
                REQUIRE_THAT(oopVec[i], Catch::Matchers::WithinRel(lhsVec[i]));
            }
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(0.0));
            marvin::math::vecops::subtract<T>(oopVec, lhsVec, static_cast<T>(10.0));
            marvin::math::vecops::subtract<T>(lhsVec, static_cast<T>(10.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsVec[i], Catch::Matchers::WithinRel(static_cast<T>(-10.0)));
                REQUIRE_THAT(oopVec[i], Catch::Matchers::WithinRel(lhsVec[i]));
            }
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(-10.0));
            std::span<T, N> oopView{ oopArr.data(), N };
            std::span<T, N> lhsView{ lhsArr.data(), N };
            std::span<T, N> rhsView{ rhsArr.data(), N };
            marvin::math::vecops::add<T>(oopView, lhsView, rhsView);
            marvin::math::vecops::add<T>(lhsView, rhsView);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsView[i], Catch::Matchers::WithinRel(static_cast<T>(-10.0)));
                REQUIRE_THAT(oopView[i], Catch::Matchers::WithinRel(lhsView[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::math::vecops::subtract<T>(oopView, lhsView, static_cast<T>(100.0));
            marvin::math::vecops::subtract<T>(lhsView, static_cast<T>(100.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsView[i], Catch::Matchers::WithinRel(static_cast<T>(-100.0)));
                REQUIRE_THAT(oopView[i], Catch::Matchers::WithinRel(lhsView[i]));
            }
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(30.0));
            marvin::math::vecops::subtract<T>(oopArr.data(), lhsArr.data(), rhsArr.data(), N);
            marvin::math::vecops::subtract<T>(lhsArr.data(), rhsArr.data(), N);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(-30.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            marvin::math::vecops::subtract<T>(oopArr.data(), lhsArr.data(), static_cast<T>(15.0), N);
            marvin::math::vecops::subtract<T>(lhsArr.data(), static_cast<T>(15.0), N);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(-15.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
        }
    }

    template <FloatType T, size_t N>
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
            marvin::math::vecops::subtract<T>(lhs, rhs);
        };
    }

    template <FloatType T, size_t N>
    void testMultiply() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(2.0));
            marvin::math::vecops::multiply<T>(oopArr, lhsArr, rhsArr);
            marvin::math::vecops::multiply<T>(lhsArr, rhsArr);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(4.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            marvin::math::vecops::multiply<T>(oopArr, lhsArr, static_cast<T>(5.0));
            marvin::math::vecops::multiply<T>(lhsArr, static_cast<T>(5.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(10.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> oopVec, lhsVec, rhsVec;
            oopVec.resize(N);
            lhsVec.resize(N);
            rhsVec.resize(N);
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(2.0));
            std::fill(rhsVec.begin(), rhsVec.end(), static_cast<T>(-1.0));
            marvin::math::vecops::multiply<T>(oopVec, lhsVec, rhsVec);
            marvin::math::vecops::multiply<T>(lhsVec, rhsVec);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsVec[i], Catch::Matchers::WithinRel(static_cast<T>(-2.0)));
                REQUIRE_THAT(oopVec[i], Catch::Matchers::WithinRel(lhsVec[i]));
            }
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(2.0));
            marvin::math::vecops::multiply<T>(oopVec, lhsVec, static_cast<T>(10.0));
            marvin::math::vecops::multiply<T>(lhsVec, static_cast<T>(10.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsVec[i], Catch::Matchers::WithinRel(static_cast<T>(20.0)));
                REQUIRE_THAT(oopVec[i], Catch::Matchers::WithinRel(lhsVec[i]));
            }
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(-10.0));
            std::span<T, N> oopView{ oopArr.data(), N };
            std::span<T, N> lhsView{ lhsArr.data(), N };
            std::span<T, N> rhsView{ rhsArr.data(), N };
            marvin::math::vecops::multiply<T>(oopView, lhsView, rhsView);
            marvin::math::vecops::multiply<T>(lhsView, rhsView);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsView[i], Catch::Matchers::WithinRel(static_cast<T>(-20.0)));
                REQUIRE_THAT(oopView[i], Catch::Matchers::WithinRel(lhsView[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            marvin::math::vecops::multiply<T>(oopView, lhsView, static_cast<T>(100.0));
            marvin::math::vecops::multiply<T>(lhsView, static_cast<T>(100.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsView[i], Catch::Matchers::WithinRel(static_cast<T>(200.0)));
                REQUIRE_THAT(oopView[i], Catch::Matchers::WithinRel(lhsView[i]));
            }
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(30.0));
            marvin::math::vecops::multiply<T>(oopArr.data(), lhsArr.data(), rhsArr.data(), N);
            marvin::math::vecops::multiply<T>(lhsArr.data(), rhsArr.data(), N);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(60.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            marvin::math::vecops::multiply<T>(oopArr.data(), lhsArr.data(), static_cast<T>(15.0), N);
            marvin::math::vecops::multiply<T>(lhsArr.data(), static_cast<T>(15.0), N);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(30.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
        }
    }

    template <FloatType T, size_t N>
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
            marvin::math::vecops::subtract<T>(lhs, rhs);
        };
    }
    template <FloatType T, size_t N>
    void testDivide() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(8.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(2.0));
            marvin::math::vecops::divide<T>(oopArr, lhsArr, rhsArr);
            marvin::math::vecops::divide<T>(lhsArr, rhsArr);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(4.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(15.0));
            marvin::math::vecops::divide<T>(oopArr, lhsArr, static_cast<T>(5.0));
            marvin::math::vecops::divide<T>(lhsArr, static_cast<T>(5.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(3.0)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> oopVec, lhsVec, rhsVec;
            oopVec.resize(N);
            lhsVec.resize(N);
            rhsVec.resize(N);
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(2.0));
            std::fill(rhsVec.begin(), rhsVec.end(), static_cast<T>(1.0));
            marvin::math::vecops::divide<T>(oopVec, lhsVec, rhsVec);
            marvin::math::vecops::divide<T>(lhsVec, rhsVec);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsVec[i], Catch::Matchers::WithinRel(static_cast<T>(2.0)));
                REQUIRE_THAT(oopVec[i], Catch::Matchers::WithinRel(lhsVec[i]));
            }
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(10.0));
            marvin::math::vecops::divide<T>(oopVec, lhsVec, static_cast<T>(2.5));
            marvin::math::vecops::divide<T>(lhsVec, static_cast<T>(2.5));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsVec[i], Catch::Matchers::WithinRel(static_cast<T>(4.0)));
                REQUIRE_THAT(oopVec[i], Catch::Matchers::WithinRel(lhsVec[i]));
            }
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(32.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(16.0));
            std::span<T, N> oopView{ oopArr.data(), N };
            std::span<T, N> lhsView{ lhsArr.data(), N };
            std::span<T, N> rhsView{ rhsArr.data(), N };
            marvin::math::vecops::divide<T>(oopView, lhsView, rhsView);
            marvin::math::vecops::divide<T>(lhsView, rhsView);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsView[i], Catch::Matchers::WithinRel(static_cast<T>(2.0)));
                REQUIRE_THAT(oopView[i], Catch::Matchers::WithinRel(lhsView[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(1000.0));
            marvin::math::vecops::divide<T>(oopView, lhsView, static_cast<T>(100.0));
            marvin::math::vecops::divide<T>(lhsView, static_cast<T>(100.0));
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsView[i], Catch::Matchers::WithinRel(static_cast<T>(10.0)));
                REQUIRE_THAT(oopView[i], Catch::Matchers::WithinRel(lhsView[i]));
            }
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> oopArr, lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(1.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(10.0));
            marvin::math::vecops::divide<T>(oopArr.data(), lhsArr.data(), rhsArr.data(), N);
            marvin::math::vecops::divide<T>(lhsArr.data(), rhsArr.data(), N);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(0.1)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(2.0));
            marvin::math::vecops::divide<T>(oopArr.data(), lhsArr.data(), static_cast<T>(4.0), N);
            marvin::math::vecops::divide<T>(lhsArr.data(), static_cast<T>(4.0), N);
            for (size_t i = 0; i < N; ++i) {
                REQUIRE_THAT(lhsArr[i], Catch::Matchers::WithinRel(static_cast<T>(0.5)));
                REQUIRE_THAT(oopArr[i], Catch::Matchers::WithinRel(lhsArr[i]));
            }
        }
    }

    template <FloatType T, size_t N>
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
            marvin::math::vecops::subtract<T>(lhs, rhs);
        };
    }

    template <FloatType T, size_t N>
    void testCopy() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(0.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(2.0));
            marvin::math::vecops::copy<T>(lhsArr, rhsArr);
            for (const auto& el : lhsArr) {
                REQUIRE_THAT(el, Catch::Matchers::WithinRel(static_cast<T>(2.0)));
            }
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> lhsVec, rhsVec;
            lhsVec.resize(N);
            rhsVec.resize(N);
            std::fill(lhsVec.begin(), lhsVec.end(), static_cast<T>(2.0));
            std::fill(rhsVec.begin(), rhsVec.end(), static_cast<T>(1.0));
            marvin::math::vecops::copy<T>(lhsVec, rhsVec);
            for (const auto& el : lhsVec) {
                REQUIRE_THAT(el, Catch::Matchers::WithinRel(static_cast<T>(1.0)));
            }
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(32.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(16.0));
            std::span<T, N> lhsView{ lhsArr.data(), N };
            std::span<T, N> rhsView{ rhsArr.data(), N };
            marvin::math::vecops::copy<T>(lhsView, rhsView);
            for (const auto& el : lhsView) {
                REQUIRE_THAT(el, Catch::Matchers::WithinRel(static_cast<T>(16.0)));
            }
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> lhsArr, rhsArr;
            std::fill(lhsArr.begin(), lhsArr.end(), static_cast<T>(1.0));
            std::fill(rhsArr.begin(), rhsArr.end(), static_cast<T>(10.0));
            marvin::math::vecops::copy<T>(lhsArr.data(), rhsArr.data(), N);
            for (const auto& el : lhsArr) {
                REQUIRE_THAT(el, Catch::Matchers::WithinRel(static_cast<T>(10.0)));
            }
        }
    }

    template <FloatType T, size_t N>
    void testSum() {
        std::string typeStr{ getTypeName<T>() };
        SECTION(fmt::format("std::array<{}, {}>", typeStr, N)) {
            std::array<T, N> arr;
            std::fill(arr.begin(), arr.end(), static_cast<T>(2.0));
            const auto sum = marvin::math::vecops::sum<T>(arr);
            REQUIRE_THAT(sum, Catch::Matchers::WithinRel(static_cast<T>(2.0) * static_cast<T>(N)));
        }
        SECTION(fmt::format("std::vector<{}>, N = {}", typeStr, N)) {
            std::vector<T> vec;
            vec.resize(N);
            std::fill(vec.begin(), vec.end(), static_cast<T>(1.0));
            const auto sum = marvin::math::vecops::sum<T>(vec);
            REQUIRE_THAT(sum, Catch::Matchers::WithinRel(static_cast<T>(N)));
        }
        SECTION(fmt::format("std::span<{}, {}>", typeStr, N)) {
            std::array<T, N> arr;
            std::fill(arr.begin(), arr.end(), static_cast<T>(32.0));
            std::span<T, N> view{ arr.data(), N };
            const auto res = marvin::math::vecops::sum<T>(view);
            REQUIRE_THAT(res, Catch::Matchers::WithinRel(static_cast<T>(32.0) * static_cast<T>(N)));
        }

        SECTION(fmt::format("{}*, N = {}", typeStr, N)) {
            std::array<T, N> arr;
            std::fill(arr.begin(), arr.end(), static_cast<T>(10.0));
            const auto res = marvin::math::vecops::sum<T>(arr.data(), arr.size());
            REQUIRE_THAT(res, Catch::Matchers::WithinRel(static_cast<T>(10.0) * static_cast<T>(N)));
        }
    }

    template <FloatType T, size_t N>
    void benchmarkSum() {
        const auto typeName = getTypeName<T>();
        std::array<T, N> arr;
        std::fill(arr.begin(), arr.end(), static_cast<T>(200.0));
        BENCHMARK(fmt::format("Sum fallback: std::array<{}, {}>", typeName, N)) {
            [[maybe_unused]] auto sum{ static_cast<T>(0.0) };
            for (size_t i = 0; i < N; ++i) {
                sum += arr[i];
            }
        };
        std::fill(arr.begin(), arr.end(), static_cast<T>(200.0));
        BENCHMARK(fmt::format("Sum SIMD: std::array<{}, {}>", typeName, N)) {
            [[maybe_unused]] auto sum = marvin::math::vecops::sum<T>(arr);
        };
    }


    TEST_CASE("Test VecOps") {

        SECTION("Test Add") {

            testAdd<float, 4>();
            testAdd<double, 4>();
            testAdd<float, 5>();
            testAdd<double, 5>();
            testAdd<float, 8>();
            testAdd<double, 8>();
            testAdd<float, 9>();
            testAdd<double, 9>();
            testAdd<float, 16>();
            testAdd<double, 16>();
            testAdd<float, 17>();
            testAdd<double, 17>();
        }
#if defined(MARVIN_ANALYSIS)
        SECTION("Benchmark Add") {
            benchmarkAdd<float, 32>();
            benchmarkAdd<double, 32>();
        }
#endif
        SECTION("Test Subtract") {
            testSubtract<float, 4>();
            testSubtract<double, 4>();
            testSubtract<float, 5>();
            testSubtract<double, 5>();
            testSubtract<float, 8>();
            testSubtract<double, 8>();
            testSubtract<float, 9>();
            testSubtract<double, 9>();
            testSubtract<float, 16>();
            testSubtract<double, 16>();
            testSubtract<float, 17>();
            testSubtract<double, 17>();
        }
#if defined(MARVIN_ANALYSIS)
        SECTION("Benchmark Subtract") {
            benchmarkSubtract<float, 32>();
            benchmarkSubtract<double, 32>();
        }
#endif
        SECTION("Test Multiply") {
            testMultiply<float, 4>();
            testMultiply<double, 4>();
            testMultiply<float, 5>();
            testMultiply<double, 5>();
            testMultiply<float, 8>();
            testMultiply<double, 8>();
            testMultiply<float, 9>();
            testMultiply<double, 9>();
            testMultiply<float, 16>();
            testMultiply<double, 16>();
            testMultiply<float, 17>();
            testMultiply<double, 17>();
        }
#if defined(MARVIN_ANALYSIS)
        SECTION("Benchmark Multiply") {
            benchmarkMultiply<float, 32>();
            benchmarkMultiply<double, 32>();
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
#if defined(MARVIN_ANALYSIS)
        SECTION("Benchmark Divide") {
            benchmarkDivide<float, 32>();
            benchmarkDivide<double, 32>();
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

        SECTION("Test Sum") {
            testSum<float, 4>();
            testSum<double, 4>();
            testSum<float, 9>();
            testSum<double, 9>();
            testSum<float, 12>();
            testSum<double, 12>();
            testSum<float, 2>();
            testSum<double, 2>();
            testSum<float, 100>();
            testSum<double, 100>();
        }

        // #if defined(MARVIN_ANALYSIS)
        //         SECTION("Benchmark Sum") {
        //             benchmarkSum<float, 32>();
        //             benchmarkSum<double, 32>();
        //         }
        // #endif
    }
} // namespace marvin::testing
