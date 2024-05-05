// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/dsp/filters/biquad/marvin_BiquadCoefficients.h"
#include "marvin/utils/marvin_SmoothedValue.h"
#include <marvin/library/marvin_Concepts.h>
#include <marvin/dsp/filters/biquad/marvin_SmoothedBiquadCoefficients.h>
#include <marvin/library/marvin_Literals.h>
#include <marvin/math/marvin_Math.h>
#include <fmt/core.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <string>
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

    template <FloatType T, size_t N>
    void testLinear() {
        const auto typeName = getTypeName<T>();
        SECTION(fmt::format("Test Linear<{}, {}>", typeName, N)) {
            constexpr static auto t{ 100 };
            marvin::dsp::filters::BiquadCoefficients<T> current{
                .a0 = static_cast<T>(0.0),
                .a1 = static_cast<T>(0.0),
                .a2 = static_cast<T>(0.0),
                .b0 = static_cast<T>(0.0),
                .b1 = static_cast<T>(0.0),
                .b2 = static_cast<T>(0.0),
            };
            marvin::dsp::filters::BiquadCoefficients<T> target{
                .a0 = static_cast<T>(1.0),
                .a1 = static_cast<T>(1.0),
                .a2 = static_cast<T>(1.0),
                .b0 = static_cast<T>(1.0),
                .b1 = static_cast<T>(1.0),
                .b2 = static_cast<T>(1.0),
            };
            dsp::filters::SmoothedBiquadCoefficients<T, marvin::utils::SmoothingType::Linear, N> coeffs;
            coeffs.reset(t);
            for (auto i = 0_sz; i < N; ++i) {
                coeffs.setCurrentAndTargetCoeffs(i, current);
                coeffs.setTargetCoeffs(i, target);
                REQUIRE(coeffs.current(i) == current);
                REQUIRE(coeffs.target(i) == target);
            }
            for (auto i = 0; i < t; ++i) {
                const auto ratio = static_cast<T>(i) / static_cast<T>(t);
                const auto interpolated = math::lerp(static_cast<T>(0.0), static_cast<T>(1.0), ratio);
                const dsp::filters::BiquadCoefficients<T> dummyCurrent = {
                    .a0 = interpolated,
                    .a1 = interpolated,
                    .a2 = interpolated,
                    .b0 = interpolated,
                    .b1 = interpolated,
                    .b2 = interpolated
                };
                for (auto stage = 0_sz; stage < N; ++stage) {
                    const auto current = coeffs.current(stage);
                    REQUIRE_THAT(current.a0, Catch::Matchers::WithinRel(dummyCurrent.a0));
                    REQUIRE_THAT(current.a1, Catch::Matchers::WithinRel(dummyCurrent.a1));
                    REQUIRE_THAT(current.a2, Catch::Matchers::WithinRel(dummyCurrent.a2));
                    REQUIRE_THAT(current.b0, Catch::Matchers::WithinRel(dummyCurrent.b0));
                    REQUIRE_THAT(current.b1, Catch::Matchers::WithinRel(dummyCurrent.b1));
                    REQUIRE_THAT(current.b2, Catch::Matchers::WithinRel(dummyCurrent.b2));
                }
                coeffs.interpolate();
            }
        }
    }

    template <FloatType T, size_t N>
    void testExponential() {
        constexpr static auto t{ 100 };
        marvin::dsp::filters::BiquadCoefficients<T> current{
            .a0 = static_cast<T>(0.0),
            .a1 = static_cast<T>(0.0),
            .a2 = static_cast<T>(0.0),
            .b0 = static_cast<T>(0.0),
            .b1 = static_cast<T>(0.0),
            .b2 = static_cast<T>(0.0),
        };
        marvin::dsp::filters::BiquadCoefficients<T> target{
            .a0 = static_cast<T>(1.0),
            .a1 = static_cast<T>(1.0),
            .a2 = static_cast<T>(1.0),
            .b0 = static_cast<T>(1.0),
            .b1 = static_cast<T>(1.0),
            .b2 = static_cast<T>(1.0),
        };
        dsp::filters::SmoothedBiquadCoefficients<T, marvin::utils::SmoothingType::Exponential, N> coeffs;
        utils::SmoothedValue<T, utils::SmoothingType::Exponential> smoother;
        coeffs.reset(t);
        smoother.reset(t);
        smoother.setCurrentAndTargetValue(current.a0);
        smoother.setTargetValue(target.a0);
        for (auto i = 0_sz; i < N; ++i) {
            coeffs.setCurrentAndTargetCoeffs(i, current);
            coeffs.setTargetCoeffs(i, target);
            REQUIRE(coeffs.current(i) == current);
            REQUIRE(coeffs.target(i) == target);
        }
        auto expected = static_cast<T>(0.0);
        for (auto i = 0; i < t; ++i) {
            const dsp::filters::BiquadCoefficients<T> dummyCurrent = {
                .a0 = expected,
                .a1 = expected,
                .a2 = expected,
                .b0 = expected,
                .b1 = expected,
                .b2 = expected
            };
            for (auto stage = 0_sz; stage < N; ++stage) {
                const auto current = coeffs.current(stage);
                REQUIRE_THAT(current.a0, Catch::Matchers::WithinRel(dummyCurrent.a0));
                REQUIRE_THAT(current.a1, Catch::Matchers::WithinRel(dummyCurrent.a1));
                REQUIRE_THAT(current.a2, Catch::Matchers::WithinRel(dummyCurrent.a2));
                REQUIRE_THAT(current.b0, Catch::Matchers::WithinRel(dummyCurrent.b0));
                REQUIRE_THAT(current.b1, Catch::Matchers::WithinRel(dummyCurrent.b1));
                REQUIRE_THAT(current.b2, Catch::Matchers::WithinRel(dummyCurrent.b2));
            }
            coeffs.interpolate();
            expected = smoother();
        }
    }

    TEST_CASE("Test SmoothedBiquadCoefficients") {
        SECTION("Test Linear") {
            testLinear<float, 1>();
            testLinear<float, 4>();
        }
        SECTION("Test Exponential") {
            testExponential<float, 1>();
            testExponential<float, 4>();
        }
    }
} // namespace marvin::testing
