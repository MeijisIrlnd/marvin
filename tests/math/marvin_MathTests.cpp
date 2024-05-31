// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "catch2/matchers/internal/catch_matchers_impl.hpp"
#include <marvin/library/marvin_Concepts.h>
#include <marvin/math/marvin_Math.h>
#include <marvin/utils/marvin_Utils.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
namespace marvin::testing {
    template <FloatType T, size_t NumSteps>
    void testLerp(T min, T max) {
        const auto increment = (max - min) / static_cast<T>(NumSteps);
        for (auto i = 0_sz; i < NumSteps; ++i) {
            const auto expected = min + (max - min) * (increment * static_cast<T>(i));
            const auto res = math::lerp(min, max, increment * static_cast<T>(i));
            REQUIRE_THAT(res, Catch::Matchers::WithinRel(expected));
        }
    };

    TEST_CASE("Test lerp") {
        testLerp<float, 10>(0.1f, 0.9f);
        testLerp<double, 100>(-0.1, 200.0);
    }

    TEST_CASE("Test Sinc") {
        for (auto i = -16; i <= 16; ++i) {
            const auto sinced = math::sinc(static_cast<float>(i));
            auto expected{ 0.0f };
            if (i == 0) {
                expected = 1.0f;
            } else {
                expected = std::sin(std::numbers::pi_v<float> * static_cast<float>(i)) / (std::numbers::pi_v<float> * static_cast<float>(i));
            }
            REQUIRE_THAT(sinced, Catch::Matchers::WithinRel(expected));
        }
    }

} // namespace marvin::testing