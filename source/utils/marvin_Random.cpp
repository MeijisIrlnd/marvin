// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/utils/marvin_Random.h"
namespace marvin::utils {
    template <RandomEngineType Engine>
    RandomGenerator<Engine>::RandomGenerator(std::random_device& rd) : m_rng(rd()) {
    }

    template <RandomEngineType Engine>
    auto RandomGenerator<Engine>::setSeed(std::seed_seq seedSequence) -> void {
        m_rng.seed(seedSequence);
    }

    template <RandomEngineType Engine>
    auto RandomGenerator<Engine>::setSeed(int seed) -> void {
        m_rng.seed(seed);
    }


    template class RandomGenerator<std::mt19937>;
    template class RandomGenerator<std::mt19937_64>;
    template class RandomGenerator<std::minstd_rand0>;
    template class RandomGenerator<std::minstd_rand>;
    template class RandomGenerator<std::ranlux24_base>;
    template class RandomGenerator<std::ranlux48_base>;
} // namespace marvin::utils
