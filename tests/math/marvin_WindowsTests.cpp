// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <limits>
#include <marvin/math/marvin_Windows.h>
#include <marvin/library/marvin_Literals.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>
namespace marvin::testing {
    template <FloatType SampleType, size_t N>
    void testSine() {
        const auto zeroRes = math::windows::sine(static_cast<SampleType>(0), static_cast<SampleType>(N));
        const auto halfRes = math::windows::sine(static_cast<SampleType>(N) / static_cast<SampleType>(2), static_cast<SampleType>(N));
        const auto fullRes = math::windows::sine(static_cast<SampleType>(N), static_cast<SampleType>(N));
        REQUIRE_THAT(zeroRes, Catch::Matchers::WithinRel(static_cast<SampleType>(0.0)));
        REQUIRE_THAT(halfRes, Catch::Matchers::WithinRel(static_cast<SampleType>(1.0)));
        // Sine of pi is zero - but with float errors, it's not *quite* zero, so here we are
        const auto expectedFull = std::sin(std::numbers::pi_v<SampleType>);
        REQUIRE_THAT(fullRes, Catch::Matchers::WithinRel(expectedFull));
    }

    template <FloatType SampleType, size_t N>
    void testTukey(SampleType alpha) {
        const auto zeroRes = math::windows::tukey(static_cast<SampleType>(0.0), static_cast<SampleType>(N), alpha);
        const auto halfRes = math::windows::tukey(static_cast<SampleType>(N) / static_cast<SampleType>(2), static_cast<SampleType>(N), alpha);
        const auto fullRes = math::windows::tukey(static_cast<SampleType>(N), static_cast<SampleType>(N), alpha);
        REQUIRE_THAT(zeroRes, Catch::Matchers::WithinRel(static_cast<SampleType>(0.0)));
        REQUIRE_THAT(halfRes, Catch::Matchers::WithinRel(static_cast<SampleType>(1.0)));
        REQUIRE_THAT(fullRes, Catch::Matchers::WithinRel(static_cast<SampleType>(0.0)));
    }

    template <FloatType SampleType, size_t N>
    void testBlackmanHarris() {
        constexpr static auto expectedZero = static_cast<SampleType>(0.35875 - 0.48829 + 0.14128 - 0.01168);
        const auto zeroRes = math::windows::blackmanHarris(static_cast<SampleType>(0.0), static_cast<SampleType>(N));
        const auto halfRes = math::windows::blackmanHarris(static_cast<SampleType>(N) / static_cast<SampleType>(2.0), static_cast<SampleType>(N));
        const auto fullRes = math::windows::blackmanHarris(static_cast<SampleType>(N), static_cast<SampleType>(N));
        REQUIRE_THAT(zeroRes, Catch::Matchers::WithinRel(expectedZero));
        REQUIRE_THAT(halfRes, Catch::Matchers::WithinRel(static_cast<SampleType>(1.0)));
        REQUIRE_THAT(fullRes, Catch::Matchers::WithinRel(expectedZero));
    }

    template <FloatType SampleType, size_t N>
    void testCosineSum(SampleType alpha) {
        // Remap from 0 to 1 to -1 to 1
        const auto expectedBoundary{ (static_cast<SampleType>(2.0) * alpha) - static_cast<SampleType>(1.0) };
        const auto zeroRes = math::windows::cosineSum(static_cast<SampleType>(0.0), static_cast<SampleType>(N), alpha);
        const auto halfRes = math::windows::cosineSum(static_cast<SampleType>(N) / static_cast<SampleType>(2.0), static_cast<SampleType>(N), alpha);
        const auto fullRes = math::windows::cosineSum(static_cast<SampleType>(N), static_cast<SampleType>(N), alpha);
        REQUIRE_THAT(zeroRes, Catch::Matchers::WithinRel(expectedBoundary));
        REQUIRE_THAT(halfRes, Catch::Matchers::WithinRel(static_cast<SampleType>(1.0)));
        REQUIRE_THAT(fullRes, Catch::Matchers::WithinRel(expectedBoundary));
    }

    template <FloatType SampleType, size_t N>
    void testHann() {
        const auto zeroRes = math::windows::hann(static_cast<SampleType>(0.0), static_cast<SampleType>(N));
        const auto halfRes = math::windows::hann(static_cast<SampleType>(N) / static_cast<SampleType>(2.0), static_cast<SampleType>(N));
        const auto fullRes = math::windows::hann(static_cast<SampleType>(N), static_cast<SampleType>(N));
        REQUIRE_THAT(zeroRes, Catch::Matchers::WithinRel(static_cast<SampleType>(0.0)));
        REQUIRE_THAT(halfRes, Catch::Matchers::WithinRel(static_cast<SampleType>(1.0)));
        REQUIRE_THAT(fullRes, Catch::Matchers::WithinRel(static_cast<SampleType>(0.0)));
    }

    template <FloatType SampleType, size_t N>
    void testHamming() {
        constexpr static auto alpha = static_cast<SampleType>(25.0 / 46.0);
        constexpr static auto expectedBoundary{ (static_cast<SampleType>(2.0) * alpha) - static_cast<SampleType>(1.0) };
        const auto zeroRes = math::windows::hamming(static_cast<SampleType>(0.0), static_cast<SampleType>(N));
        const auto halfRes = math::windows::hamming(static_cast<SampleType>(N) / static_cast<SampleType>(2.0), static_cast<SampleType>(N));
        const auto fullRes = math::windows::hamming(static_cast<SampleType>(N), static_cast<SampleType>(N));
        REQUIRE_THAT(zeroRes, Catch::Matchers::WithinRel(expectedBoundary));
        REQUIRE_THAT(halfRes, Catch::Matchers::WithinRel(static_cast<SampleType>(1.0)));
        REQUIRE_THAT(fullRes, Catch::Matchers::WithinRel(expectedBoundary));
    }

    template <FloatType SampleType, size_t N, math::windows::WindowType Type>
    void testSymmetry() {
        constexpr static auto epsilon{ static_cast<SampleType>(1e-6) };
        SampleType lhs{ static_cast<SampleType>(0.0) };
        SampleType rhs{ static_cast<SampleType>(0.0) };
        constexpr static auto stSize{ static_cast<SampleType>(N) };
        for (auto i = 0_sz; i < N / 2; ++i) {
            const auto stI = static_cast<SampleType>(i);
            const auto reverseIndex = stSize - stI;
            if constexpr (Type == math::windows::WindowType::Sine) {
                lhs = math::windows::sine(stI, stSize);
                rhs = math::windows::sine(reverseIndex, stSize);
            } else if constexpr (Type == math::windows::WindowType::Tukey) {
                lhs = math::windows::tukey(stI, stSize, static_cast<SampleType>(0.5));
                rhs = math::windows::tukey(reverseIndex, stSize, static_cast<SampleType>(0.5));
            } else if constexpr (Type == math::windows::WindowType::CosineSum) {
                lhs = math::windows::cosineSum(stI, stSize, static_cast<SampleType>(0.0));
                rhs = math::windows::cosineSum(reverseIndex, stSize, static_cast<SampleType>(0.0));
            } else if constexpr (Type == math::windows::WindowType::Hann) {
                lhs = math::windows::hann(stI, stSize);
                rhs = math::windows::hann(reverseIndex, stSize);
            } else if constexpr (Type == math::windows::WindowType::Hamming) {
                lhs = math::windows::hamming(stI, stSize);
                rhs = math::windows::hamming(reverseIndex, stSize);
            } else {
                lhs = math::windows::blackmanHarris(stI, stSize);
                rhs = math::windows::blackmanHarris(reverseIndex, stSize);
            }
            const auto delta = std::abs(lhs - rhs);
            REQUIRE(delta <= epsilon);
        }
    }

    TEST_CASE("Test Windowing Functions") {
        testSine<float, 100>();
        testTukey<float, 100>(0.5f);
        testBlackmanHarris<float, 100>();
        testCosineSum<float, 100>(0.0f);
        testCosineSum<float, 100>(0.25f);
        testHann<float, 100>();
        testHamming<float, 100>();

        testSymmetry<float, 64, math::windows::WindowType::Sine>();
        testSymmetry<float, 64, math::windows::WindowType::Tukey>();
        testSymmetry<float, 64, math::windows::WindowType::BlackmanHarris>();
        testSymmetry<float, 64, math::windows::WindowType::CosineSum>();
        testSymmetry<float, 64, math::windows::WindowType::Hann>();
        testSymmetry<float, 64, math::windows::WindowType::Hamming>();

        testSine<double, 100>();
        testTukey<double, 100>(0.5);
        testBlackmanHarris<double, 100>();
        testCosineSum<double, 100>(0.0);
        testCosineSum<double, 100>(0.25);
        testHann<double, 100>();
        testHamming<double, 100>();

        testSymmetry<double, 64, math::windows::WindowType::Sine>();
        testSymmetry<double, 64, math::windows::WindowType::Tukey>();
        testSymmetry<double, 64, math::windows::WindowType::BlackmanHarris>();
        testSymmetry<double, 64, math::windows::WindowType::CosineSum>();
        testSymmetry<double, 64, math::windows::WindowType::Hann>();
        testSymmetry<double, 64, math::windows::WindowType::Hamming>();
    }

} // namespace marvin::testing
