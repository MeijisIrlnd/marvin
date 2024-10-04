// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================
#include <marvin/library/marvin_Concepts.h>
#include <marvin/math/marvin_Conversions.h>
#include <marvin/utils/marvin_Random.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
namespace marvin::testing {
    static std::random_device s_rd;
    template <marvin::FloatType T>
    void testMsToSeconds(T ms, T expected) {
        const auto actual = math::msToSeconds(ms);
        REQUIRE_THAT(actual, Catch::Matchers::WithinRel(expected));
    }

    template <marvin::FloatType T>
    void testSecondsToSamples(T seconds, double sampleRate, T expected) {
        const auto actual = math::secondsToSamples(seconds, sampleRate);
        REQUIRE_THAT(actual, Catch::Matchers::WithinRel(expected));
    }

    template <marvin::FloatType T>
    void testMsToSamples(T ms, double sampleRate, T expected) {
        const auto actual = math::msToSamples(ms, sampleRate);
        REQUIRE_THAT(actual, Catch::Matchers::WithinRel(expected));
    }

    static constexpr auto s_numIterations{ 100 };
    static constexpr auto s_sampleRate{ 44100.0 };

    TEST_CASE("Test MS to Sec") {
        marvin::utils::Random rng{ s_rd };
        marvin::utils::Range<float> range{ 0.0f, 10000.0f };
        for (auto i = 0; i < s_numIterations; ++i) {
            const auto rand = rng.generate(range);
            const auto expected = rand / 1000.0f;
            testMsToSeconds(rand, expected);
        }
    }

    TEST_CASE("Test Sec to Samples") {
        marvin::utils::Random rng{ s_rd };
        marvin::utils::Range range{ 0.0f, 10000.0f };
        for (auto i = 0; i < s_numIterations; ++i) {
            const auto rand = rng.generate(range);
            const auto expected = rand * static_cast<float>(s_sampleRate);
            testSecondsToSamples(rand, s_sampleRate, expected);
        }
    }

    TEST_CASE("Test MS to Samples") {
        marvin::utils::Random rng{ s_rd };
        marvin::utils::Range range{ 0.0f, 10000.0f };
        for (auto i = 0; i < s_numIterations; ++i) {
            const auto rand = rng.generate(range);
            const auto expected = (rand / 1000.0f) * static_cast<float>(s_sampleRate);
            testMsToSamples(rand, s_sampleRate, expected);
        }
    }


} // namespace marvin::testing