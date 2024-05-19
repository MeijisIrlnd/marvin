// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/containers/marvin_FIFO.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <type_traits>
#include <thread>
namespace marvin::testing {
    template <typename T>
    void testSPSCSingleThreaded(std::vector<T>&& data) {
        SECTION("Test push and emplace") {
            containers::fifos::SPSC<T> fifo{ data.size() };
            for (auto& d : data) {
                auto tcpy = d;
                fifo.tryPush(std::move(tcpy));
            }
            std::optional<T> popped{};
            size_t idx{ 0 };
            do {
                popped = fifo.tryPop();
                if (!popped.has_value()) continue;
                if constexpr (std::is_floating_point_v<T>) {
                    REQUIRE_THAT(*popped, Catch::Matchers::WithinRel(data[idx]));
                } else {
                    REQUIRE(*popped == data[idx]);
                }
                ++idx;
            } while (popped.has_value());
            popped = fifo.tryPop();
            REQUIRE(!popped);
        }
        SECTION("Test empty queue") {
            containers::fifos::SPSC<T> fifo{ data.size() };
            for (auto& d : data) {
                auto tcpy = d;
                fifo.tryPush(std::move(tcpy));
            }
            fifo.emptyQueue();
            std::optional<T> tmp = fifo.tryPop();
            REQUIRE(!tmp);
        }
    }

    template <typename T>
    void testSPSCMultiThreaded(std::vector<T>&& data) {
        std::vector<T> dequeuedData;
        std::atomic_bool threadShouldExit{ false };
        marvin::containers::fifos::SPSC<T> fifo{ data.size() };
        auto threadTask = [](std::atomic_bool& shouldExitSignal, marvin::containers::fifos::SPSC<T>& fifo, std::vector<T>& dequeuedData) -> void {
            do {
                std::optional<T> dr;
                size_t idx{ 0 };
                do {
                    dr = fifo.tryPop();
                    if (!dr) continue;
                    dequeuedData.emplace_back(*dr);
                } while (dr.has_value());
            } while (!shouldExitSignal);
        };
        std::thread execThread{ std::move(threadTask), std::ref(threadShouldExit), std::ref(fifo), std::ref(dequeuedData) };
        for (const auto& el : data) {
            auto temp = el;
            fifo.tryPush(std::move(temp));
        }
        threadShouldExit.store(true);
        execThread.join();
        REQUIRE(dequeuedData.size() == data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            REQUIRE(dequeuedData[i] == data[i]);
        }
        REQUIRE(!fifo.tryPop());
    }


    TEST_CASE("Test FIFOs") {
        SECTION("Test SPSC") {
            testSPSCSingleThreaded<int>({});
            testSPSCSingleThreaded<int>({ 0, 4, 2, 3, 7 });
            testSPSCSingleThreaded<float>({ 19.0f, 3.0f, 20.0f, 0.0f, 0.0f });
            testSPSCSingleThreaded<double>({ 10.0, 3.0, 0.0, 9.0, 100.0, 25.0, 99.0 });
            testSPSCSingleThreaded<std::string>({ "aaa", "bbb", "ccc", "ddd", "eee", "fff" });
            testSPSCMultiThreaded<int>({ 0, 15, 25, 0, 2, 3, 8, 9, 10, 2, 3 });
            testSPSCMultiThreaded<int>({ 0, 4, 2, 3, 7 });
            testSPSCMultiThreaded<float>({ 19.0f, 3.0f, 20.0f, 0.0f, 0.0f });
            testSPSCMultiThreaded<double>({ 10.0, 3.0, 0.0, 9.0, 100.0, 25.0, 99.0 });
            testSPSCMultiThreaded<std::string>({ "aaa", "bbb", "ccc", "ddd", "eee", "fff" });
        }
    }
} // namespace marvin::testing
