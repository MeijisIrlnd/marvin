// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_MIXMATRIX_H
#define MARVIN_MIXMATRIX_H
#include "marvin/library/marvin_Concepts.h"
#include <cmath>

namespace marvin::utils {
    // Use like `Householder<double, 8>::inPlace(data)` - size must be ≥ 1
    template <FloatType SampleType, int size>
    requires(size >= 1)
    class Householder {
        static constexpr SampleType multiplier{ -2.0 / size };

    public:
        static void inPlace(SampleType* arr) {
            SampleType sum = 0;
            for (int i = 0; i < size; ++i) {
                sum += arr[i];
            }
            sum *= multiplier;
            for (int i = 0; i < size; ++i) {
                arr[i] += sum;
            }
        }
    };

    // Use like `Hadamard<double, 8>::inPlace(data)` - size must be a power of 2
    template <FloatType SampleType, int size>
    requires(isPowerOfTwo<size>())
    class Hadamard {
    public:
        static inline void recursiveUnscaled(SampleType* data) {
            if constexpr (size <= 1)
                return;
            else {
                constexpr int hSize = size / 2;
                // Two (unscaled) Hadamards of half the size
                Hadamard<SampleType, hSize>::recursiveUnscaled(data);
                Hadamard<SampleType, hSize>::recursiveUnscaled(data + hSize);
                // Combine the two halves using sum/difference
                for (int i = 0; i < hSize; ++i) {
                    SampleType a = data[i];
                    SampleType b = data[i + hSize];
                    data[i] = (a + b);
                    data[i + hSize] = (a - b);
                }
            }
        }
        static inline void inPlace(SampleType* data) {
            recursiveUnscaled(data);
            auto scalingFactor = static_cast<SampleType>(std::sqrt(1.0 / size));
            for (int c = 0; c < size; ++c) {
                data[c] *= scalingFactor;
            }
        }
    };
} // namespace marvin::utils
#endif