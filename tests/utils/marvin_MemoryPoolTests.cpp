// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================
#include <marvin/utils/marvin_MemoryPool.h>
#include <catch2/catch_test_macros.hpp>
namespace marvin::testing {
    struct TestStruct {
        std::string data[64];
        int x;
    };

    TEST_CASE("Test MemoryPool") {
        {
            utils::MemoryPool<int> pool{ 512 };
            auto* a1 = pool.alloc(1);
            REQUIRE(a1);
            REQUIRE(*a1 == 1);
            // free without nulling..
            pool.free(a1);
            // Check that we're using the same memory address..
            auto a2 = pool.alloc(10);
            REQUIRE(a2 == a1);
            // Check that alloc creates a new arena..
            for (auto i = 0; i < 513; ++i) {
                auto* temp = pool.alloc(i);
                REQUIRE(temp);
            }
            pool.clear();
        }
        {
            utils::MemoryPool<float> pool{ 2 };
            REQUIRE(pool.hasFreeSpace());
            auto* a = pool.alloc();
            auto* b = pool.alloc();
            REQUIRE(!pool.hasFreeSpace());
            auto c = pool.tryAlloc();
            REQUIRE(!c);
            pool.free(a);
            REQUIRE(pool.hasFreeSpace());
            auto d = pool.tryAlloc();
            REQUIRE(d);
            REQUIRE(!pool.hasFreeSpace());
        }
        {
            utils::MemoryPool<TestStruct> pool{ 10 };
            auto* res = pool.alloc(TestStruct{ .data = "Hello world!", .x = 100 });
            REQUIRE(res);
            REQUIRE(res->x == 100);
            pool.free(res);
        }
    }
} // namespace marvin::testing