// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/utils/marvin_SmoothedValue.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <fmt/format.h>
#include <numbers>
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

    template <FloatType T>
    void testLinear(T start, T end) {
        marvin::utils::SmoothedValue<T, marvin::utils::SmoothingType::Linear> smoothedValue;
        smoothedValue.reset(100);
        smoothedValue.setCurrentAndTargetValue(start);
        smoothedValue.setTargetValue(end);
        const auto expectedIncrement{ (end - start) / static_cast<T>(100.0) };
        auto expected{ start };
        for (auto i = 1; i < 101; ++i) {
            REQUIRE(smoothedValue.isSmoothing());
            const auto nextSmoothed = smoothedValue();
            expected += expectedIncrement;
            REQUIRE_THAT(nextSmoothed, Catch::Matchers::WithinRel(expected));
        }
        REQUIRE(!smoothedValue.isSmoothing());
    }

    template <FloatType T>
    void testExponential(T start, T end) {
        const auto typeName = getTypeName<T>();
        SECTION(fmt::format("Test Exponential<{}>, s = {}, e = {}", typeName, start, end)) {
            constexpr static auto period{ 100 };
            marvin::utils::SmoothedValue<T, marvin::utils::SmoothingType::Exponential> smoothedValue;
            smoothedValue.reset(period);
            smoothedValue.setCurrentAndTargetValue(start);
            smoothedValue.setTargetValue(end);
            const auto targetPc = (static_cast<T>(1.0) - (static_cast<T>(1.0) / std::numbers::e_v<T>)) * static_cast<T>(100.0);
            const auto expectedAfterT = (((end - start) / static_cast<T>(100.0)) * static_cast<T>(targetPc)) + start;
            auto current{ static_cast<T>(0.0) };
            for (auto i = 0; i < period; ++i) {
                current = smoothedValue();
            }
            REQUIRE(smoothedValue.isSmoothing());
            REQUIRE_THAT(current, Catch::Matchers::WithinRel(expectedAfterT, static_cast<T>(0.01)));
            const auto remaining = smoothedValue.getRemainingSamples();
            for (auto i = 0; i <= remaining; ++i) {
                current = smoothedValue();
            }
            REQUIRE_THAT(current, Catch::Matchers::WithinRel(end, static_cast<T>(0.01)));
            REQUIRE(!smoothedValue.isSmoothing());
        }
    }

    TEST_CASE("Test SmoothedValue") {
        SECTION("Test linear") {
            testLinear<float>(0.0f, 1.0f);
            testLinear<float>(2.0f, 100.0f);
            testLinear<float>(-1.0f, 1000.0f);
            testLinear<float>(-1000.0f, 1000.0f);
            testLinear<float>(0.0f, 0.0f);
            testLinear<double>(0.0, 1.0);
            testLinear<double>(2.0, 100.0);
            testLinear<double>(-1.0, 1000.0);
            testLinear<double>(-1000.0, 1000.0);
            testLinear<double>(0.0, 0.0);
        }
        SECTION("Test exponential") {
            testExponential<double>(0.0, 1.0);
            testExponential<double>(0.0, 100.0);
            testExponential<double>(0.0, 1000.0);
            testExponential<double>(10.0, 1000.0);
            testExponential<double>(20.0, -20.0);
            testExponential<double>(0.0, 0.0);
            testExponential<double>(-30.0, 100.0);
            testExponential<float>(0.0f, 1.0f);
            testExponential<float>(0.0f, 100.0f);
            testExponential<float>(0.0f, 1000.0f);
            testExponential<float>(10.0f, 1000.0f);
            testExponential<float>(20.0f, -20.0f);
            testExponential<float>(0.0f, 0.0f);
            testExponential<float>(-30.0f, 100.0f);
        }
    }
} // namespace marvin::testing
