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
#include <random>
namespace marvin::utils {
    /**
        \brief A class for (pseudo) random number generation.

        Internally, this class is just a wrapper around the `std::mt19937` Mersenne-Twister engine, for convenience and brevity of typing.
    */
    class Random final {
    public:
        /**
            \brief A POD type specifying the min and max range an instance of the Random class should use for generation.
        */
        template <NumericType T>
        struct Range {
            T min, max;
        };

        /**
            As it's not recommended to have multiple copies of a `std::random_device`, this class relies on one to be provided to its constructor, which is used to configure the internal `std::mt19937` rng.
            \param rd An instance of a `std::random_device` to seed the internal rng with.
        */
        explicit Random(std::random_device& rd);

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
        std::mt19937 m_rng;
    };


} // namespace marvin::utils
#endif
