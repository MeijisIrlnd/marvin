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
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::utils {
    class Random final {
    public:
        template <NumericType T>
        struct Range {
            T min, max;
        };

        explicit Random(std::random_device& rd);

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
#include "marvin/library/marvin_DisableWarnings.h"
#endif
