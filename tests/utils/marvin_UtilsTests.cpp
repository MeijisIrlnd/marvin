// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/utils/marvin_Utils.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>
namespace marvin::testing { 
    TEST_CASE("Test getCurrentExecutablePath()") { 
        const auto pathRes = utils::getCurrentExecutablePath();
        REQUIRE(pathRes);
        std::cout << *pathRes << "\n";
    }

}
