// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/library/marvin_Literals.h>
#include <marvin/containers/marvin_FixedCircularBuffer.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
namespace marvin::testing {
    template <typename T, size_t N>
    std::vector<T> generateFixedCircularBufferImpulse() {
        std::vector<T> res(N, static_cast<T>(0.0));
        res.front() = static_cast<T>(1.0);
        return res;
    }

    template <typename T, size_t N, size_t Delay>
    void testFixedCircularBuffer() {
        using namespace marvin::literals;
        containers::FixedCircularBuffer<T, N * 2> circularBuffer{};
        const auto impulse = generateFixedCircularBufferImpulse<T, N>();
        std::vector<T> res{};
        for (auto i = 0_sz; i < N; ++i) {
            auto delayed = circularBuffer.peek(Delay);
            circularBuffer.push(impulse[i]);
            res.emplace_back(delayed);
        }
        REQUIRE_THAT(res[Delay], Catch::Matchers::WithinRel(static_cast<T>(1.0)));
    }

    TEST_CASE("Test FixedCircularBuffer") {
        testFixedCircularBuffer<float, 4, 1>();
        testFixedCircularBuffer<float, 5, 3>();
        testFixedCircularBuffer<float, 8, 4>();
        testFixedCircularBuffer<float, 9, 6>();
        testFixedCircularBuffer<float, 16, 7>();
        testFixedCircularBuffer<float, 17, 11>();
        testFixedCircularBuffer<float, 32, 13>();
        testFixedCircularBuffer<float, 33, 19>();
        testFixedCircularBuffer<float, 64, 7>();
        testFixedCircularBuffer<float, 65, 1>();
        testFixedCircularBuffer<float, 128, 51>();
        testFixedCircularBuffer<float, 129, 47>();
        testFixedCircularBuffer<float, 256, 16>();
        testFixedCircularBuffer<float, 256, 255>();

        testFixedCircularBuffer<double, 4, 1>();
        testFixedCircularBuffer<double, 5, 3>();
        testFixedCircularBuffer<double, 8, 4>();
        testFixedCircularBuffer<double, 9, 6>();
        testFixedCircularBuffer<double, 16, 7>();
        testFixedCircularBuffer<double, 17, 11>();
        testFixedCircularBuffer<double, 32, 13>();
        testFixedCircularBuffer<double, 33, 19>();
        testFixedCircularBuffer<double, 64, 7>();
        testFixedCircularBuffer<double, 65, 1>();
        testFixedCircularBuffer<double, 128, 51>();
        testFixedCircularBuffer<double, 129, 47>();
        testFixedCircularBuffer<double, 256, 16>();
        testFixedCircularBuffer<double, 256, 255>();
    }

} // namespace marvin::testing
