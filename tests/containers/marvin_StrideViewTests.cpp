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
#include <marvin/library/marvin_Literals.h>
#include <marvin/containers/marvin_StrideView.h>
#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>
#include <iostream>
namespace marvin::testing {
    struct IterTestStruct {
        [[nodiscard]] bool doSomething() {
            return true;
        }
    };

    void verifyComparisons() {
        std::array<IterTestStruct, 20> testStructArr;
        containers::StrideView<IterTestStruct, 2> testStructView{ testStructArr };
        REQUIRE((*testStructView.begin()).doSomething());
        REQUIRE(testStructView.begin()->doSomething());
        REQUIRE(testStructView.begin());
        REQUIRE(testStructView.begin() == testStructView.begin());
        REQUIRE_FALSE(testStructView.begin() == testStructView.end());
        REQUIRE(testStructView.begin() < testStructView.end());
        REQUIRE(testStructView.begin() != testStructView.end());
        REQUIRE(testStructView.begin() >= testStructView.begin());
        REQUIRE(testStructView.begin() <= testStructView.begin());
        REQUIRE(testStructView.begin() + static_cast<std::ptrdiff_t>(1) == ++testStructView.begin());
        auto offsetIt = testStructView.begin() + static_cast<std::ptrdiff_t>(1);
        REQUIRE(offsetIt - static_cast<std::ptrdiff_t>(1) == testStructView.begin());
        auto* beginp{ &(*testStructView.begin()) };
        auto* offsetp{ &(*offsetIt) };
        REQUIRE(offsetIt - testStructView.begin() == offsetp - beginp);
    }

    template <NumericType T, size_t Stride>
    void verifyContainer(containers::StrideView<T, Stride> strideView, std::span<T> underlyingView) {
        const auto expectedNumElements = underlyingView.size() / Stride;
        for (auto i = 0_sz; i < expectedNumElements; ++i) {
            REQUIRE(strideView[i] == underlyingView[Stride * i]);
        }

        for (auto it = strideView.begin(); it != strideView.end(); ++it) {
            const auto dist = std::distance(strideView.begin(), it);
            const auto actualIndex = dist * Stride;
            REQUIRE(*it == underlyingView[actualIndex]);
        }

        size_t i = 0;
        for (auto& el : strideView) {
            const auto expectedEl = underlyingView[Stride * i];
            REQUIRE(el == expectedEl);
            ++i;
        }
    }

    template <NumericType T, size_t Stride, size_t N>
    void testStrideView() {
        using namespace marvin::literals;
        std::vector<T> vec(N, static_cast<T>(0.0));
        std::array<T, N> arr;
        T rawArr[N];
        for (auto i = 0_sz; i < N; ++i) {
            vec[i] = static_cast<T>(i);
            arr[i] = static_cast<T>(i);
            rawArr[i] = static_cast<T>(i);
        }
        std::span<T> sp{ arr };
        // Fucking apple clang..
        auto* vecBegin = &(*vec.begin());
        std::span<T> vecIterView{ vecBegin, N / Stride };
        containers::StrideView<T, Stride> vecView{ vec };
        containers::StrideView<T, Stride> arrayView{ arr };
        containers::StrideView<T, Stride> spanView{ sp };
        containers::StrideView<T, Stride> rawArrView{ rawArr, N };
        containers::StrideView<T, Stride> iterView{ vec.begin(), vec.begin() + static_cast<std::ptrdiff_t>(N / 2) };
        verifyContainer<T, Stride>(vecView, vec);
        verifyContainer<T, Stride>(arrayView, arr);
        verifyContainer<T, Stride>(spanView, sp);
        verifyContainer<T, Stride>(rawArrView, { rawArr, N });
        verifyContainer<T, Stride>(iterView, vecIterView);
    }
    TEST_CASE("Test StrideView") {
        verifyComparisons();
        testStrideView<int, 2, 32>();
        testStrideView<int, 3, 33>();
        testStrideView<int, 5, 150>();
        testStrideView<float, 2, 32>();
        testStrideView<float, 3, 33>();
        testStrideView<float, 5, 150>();
        testStrideView<double, 2, 32>();
        testStrideView<double, 3, 33>();
        testStrideView<double, 5, 150>();
    }
} // namespace marvin::testing
