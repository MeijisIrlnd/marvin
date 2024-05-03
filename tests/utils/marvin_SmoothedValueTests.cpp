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
#include <iostream>
namespace marvin::testing {
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
        // If your slew factor is slew, the distance to the target is (1 - slew)^n after the nth iteration.
        // If you want that distance to be 1%, then you want (1 - slew)^n = 0.01.
        // Take the log of both sides: n*log(1 - slew) = log(0.01)
        // So your n = log(0.01)/log(1 - slew), and then round up to get a whole number of iterations.
        // marvin::utils::SmoothedValue<T, marvin::utils::SmoothingType::Exponential> smoothedValue;
        // constexpr static auto period{ 100 };
        // const auto slew = static_cast<T>(1.0) / (static_cast<T>(period) + static_cast<T>(1.0));
        // // constexpr static auto delta{ static_cast<T>(1e-6) };
        // smoothedValue.reset(period);
        // smoothedValue.setCurrentAndTargetValue(start);
        // smoothedValue.setTargetValue(end);
        // auto smoothedRes = static_cast<T>(0.0);
        // for (auto i = 0; i < 100; ++i) {
        //     REQUIRE(smoothedValue.isSmoothing());
        //     smoothedRes = smoothedValue();
        //     std::cout << smoothedRes << "\n";
        // }
        // const auto tcTarget = ((end - start) / static_cast<T>(100.0)) * static_cast<T>(63.2);
        // // REQUIRE_THAT(smoothedRes, Catch::Matchers::WithinRel(tcTarget));
        // REQUIRE(!smoothedValue.isSmoothing());
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
            testExponential<float>(0.0f, 1.0f);
            testExponential<float>(0.0f, 100.0f);
        }
    }
} // namespace marvin::testing
