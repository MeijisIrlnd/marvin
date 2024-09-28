// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef SLMADSP_RANDOM_H
#define SLMADSP_RANDOM_H
#include "marvin/library/marvin_Concepts.h"
#include "marvin/utils/marvin_Range.h"
#include <random>
namespace marvin::utils {
    /**
        \brief A class for (pseudo) random number generation.

        This class is a wrapper around the usual modern c++ random approach, with a templated Engine type.
        Below is a copy pasted excerpt from the cppreference information about the random engine types:
        number of trade-offs:<br><br>

        The linear congruential engine (`std::minstd_rand`, `std::minstd_rand0`) is moderately fast and has a very small storage requirement for state.<br>
        The Mersenne twister engine (`std::mt19937`, std::mt19937_64`) is slower and has greater state storage requirements but with the right parameters has the longest non-repeating sequence with the most desirable spectral characteristics (for a given definition of desirable).<br>
        The subtract with carry engine (`std::ranlux24_base`, std::ranlux48_base`) is very fast even on processors without advanced arithmetic instruction sets, at the expense of greater state storage and sometimes less desirable spectral characteristics.<br>

    */
    template <RandomEngineType Engine>
    class RandomGenerator final {
    public:
        /**
            As it's not recommended to have multiple copies of a `std::random_device`, this class relies on one to be provided to its constructor, which is used to configure the internal `std::mt19937` rng.
            \param rd An instance of a `std::random_device` to seed the internal rng with.
        */
        explicit RandomGenerator(std::random_device& rd);

        /**
            Generates a (pseudo) random number in the given Range. Internally uses the appropriate `std::uniform_t_distribution` depending on if T is an integral type, or a float-type.
            \param range An instance of the Range POD type, specifying the min and max values the rng should generate between.
            \return A pseudo random number in `range`.
        */
        template <NumericType T>
        [[nodiscard]] T generate(Range<T> range) noexcept {
            Distribution<T> distribution{ range.min, range.max };
            const auto res = distribution(m_rng);
            return res;
        }


    private:
        template <class T>
        using Distribution = std::conditional_t<std::is_floating_point_v<T>,
                                                std::uniform_real_distribution<T>,
                                                std::uniform_int_distribution<T>>;
        Engine m_rng;
    };

    // Default, to not break existing codebases..
    using Random = RandomGenerator<std::mt19937>;


} // namespace marvin::utils
#endif
