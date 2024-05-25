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
#include <marvin/math/marvin_Math.h>
#include <marvin/utils/marvin_Utils.h>
#include <catch2/catch_test_macros.hpp>
namespace marvin::testing {
    template <FloatType T>
    void testLerp(){

    };

    TEST_CASE("Test lerp") {
    }

    TEST_CASE("Test Sinc") {
        std::vector<float> results{};
        for (auto i = -16; i <= 16; ++i) {
            const auto sinced = math::sinc(static_cast<float>(i));
            results.emplace_back(sinced);
        }
        marvin::utils::writeToCsv<float>("C:/Users/Syl/Desktop/SincGraph.csv", results);
    }

} // namespace marvin::testing