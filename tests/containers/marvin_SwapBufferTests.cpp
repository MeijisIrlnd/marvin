// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <concepts>
#include <marvin/containers/marvin_SwapBuffer.h>
#include <marvin/utils/marvin_Range.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <type_traits>
#include <complex>
namespace marvin::testing {
    template <typename T, size_t N>
    requires std::is_default_constructible_v<T> &&
             std::is_copy_constructible_v<T> &&
             std::is_move_constructible_v<T> &&
             std::equality_comparable<T>
    void testSwapBuffer(T fillA, T fillB) {
        audiobasics::SwapBuffer<T> swapBuffer{ N };

        REQUIRE(swapBuffer.size() == N);
        REQUIRE(swapBuffer.getFrontBuffer().size() == N);
        REQUIRE(swapBuffer.getBackBuffer().size() == N);

        std::fill(swapBuffer.getFrontBuffer().begin(), swapBuffer.getFrontBuffer().end(), fillA);
        std::fill(swapBuffer.getBackBuffer().begin(), swapBuffer.getBackBuffer().end(), fillB);
        swapBuffer.swap();
        for (auto& el : swapBuffer.getFrontBuffer()) {
            if constexpr (std::is_floating_point_v<T>) {
                REQUIRE_THAT(el, Catch::Matchers::WithinRel(fillB));
            } else {
                REQUIRE(el == fillB);
            }
        }
        for (auto& el : swapBuffer.getBackBuffer()) {
            if constexpr (std::is_floating_point_v<T>) {
                REQUIRE_THAT(el, Catch::Matchers::WithinRel(fillA));
            } else {
                REQUIRE(el == fillA);
            }
        }

        swapBuffer.swap();
        for (auto& el : swapBuffer.getFrontBuffer()) {
            if constexpr (std::is_floating_point_v<T>) {
                REQUIRE_THAT(el, Catch::Matchers::WithinRel(fillA));
            } else {
                REQUIRE(el == fillA);
            }
        }

        for (auto& el : swapBuffer.getBackBuffer()) {
            if constexpr (std::is_floating_point_v<T>) {
                REQUIRE_THAT(el, Catch::Matchers::WithinRel(fillB));
            } else {
                REQUIRE(el == fillB);
            }
        }

        swapBuffer.resize(N / 2);
        REQUIRE(swapBuffer.size() == N / 2);
        REQUIRE(swapBuffer.getFrontBuffer().size() == N / 2);
        REQUIRE(swapBuffer.getBackBuffer().size() == N / 2);
        {
            audiobasics::SwapBuffer<T> test{};
            REQUIRE(test.size() == 0);
            test.reserve(N);
            REQUIRE(test.size() == 0);
            test.resize(N);
            REQUIRE(test.size() == N);
        }
    }

    TEST_CASE("Test SwapBuffer") {
        testSwapBuffer<float, 512>(0.0f, 1.0f);
        testSwapBuffer<double, 129>(100.0, 10.0);
        testSwapBuffer<int, 1024>(2, 4);
        testSwapBuffer<size_t, 1024>(2, 4);
        testSwapBuffer<std::complex<float>, 300>({ 5.0f, 0.2f }, { 16.0f, 0.3f });
        testSwapBuffer<marvin::utils::Range<double>, 15>({ 30.0, 10.0 }, { 0.0, 1.0 });
    }
} // namespace marvin::testing