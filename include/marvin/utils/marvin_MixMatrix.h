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
    /**
        \brief A helper class to apply an NxN Householder matrix to a given input array-like.

        `size` <b>must</b> be greater than or equal to 1.
        Example usage:
        ```cpp
            std::array<float, 4> arr{ 1, 0, 0, 0 };
            marvin::utils::Householder<float, 4>::inPlace(arr.data());
        ```
    */
    template <FloatType SampleType, int size>
    requires(size >= 1)
    class Householder {
        static constexpr SampleType multiplier{ -2.0 / size };

    public:
        /**
            Multiplies a given input of `size` by a `size x size` Householder matrix.
            \param arr A pointer to the internal data of an array-like to apply the Householder matrix to.
        */
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

    /**
        \brief A helper class to apply an NxN Hadamard matrix to a given input array-like.

        `size` <b>must</b> be a power of two.
        Example usage:
        ```cpp
            std::array<float, 4> arr{ 1, 0, 0, 0 };
            marvin::utils::Hadamard<float, 4>::inPlace(arr.data());
        ```
    */
    template <FloatType SampleType, int size>
    requires(isPowerOfTwo<size>())
    class Hadamard {
    public:
        /**
            Used internally by `inPlace` to generate an `NxN` Hadamard via recursion - probably shouldn't be in the public API.
            \param data A pointer to the internal data of the array-like to apply the Householder to.
        */
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
        /**
            Multiplies an input array-like of `size` by a `size x size` Hadamard matrix.
            \param data A pointer to the internal data of the array-like to apply the Hadamard to.
        */
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