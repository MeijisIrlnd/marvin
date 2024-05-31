// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_MATHWINDOWS_H
#define MARVIN_MATHWINDOWS_H
#include <marvin/library/marvin_Concepts.h>
#include <numbers>
#include <cmath>
namespace marvin::math::windows {

    /**
        Represents a type of window function, for use in classes and functions which take the window type as a template argument.
    */
    enum class WindowType {
        Sine,
        Tukey
    };

    /**
        A Sine window starting at 0, as opposed to the cosine equivalent starting at -1.
        \param n The current index into the window.
        \param N The total number of points in the window - `n` will be normalised by this.
        \return A point n/N of the way along a sine window.
    */
    template <FloatType SampleType>
    [[nodiscard]] SampleType sine(SampleType n, SampleType N) {
        static constexpr auto pi = std::numbers::pi_v<SampleType>;
        const auto window = std::sin((pi * n) / N);
        return window;
    }

    /**
        A Tukey window. The response of the window is determined by the value of alpha - at `alpha = 0`, the resulting window is identical to
        a rectangular window. At `alpha = 1`, the resulting window is identical to a hann window.
        \param n The current index into the window.
        \param N The total number of points in the window - `n` will be normalised by this.
        \param alpha The "shape" param of the windowing function, between 0 and 1.
        \return A point n/N of the way along a tukey window, with a shape determined by `alpha`.
    */
    template <FloatType SampleType>
    [[nodiscard]] SampleType tukey(SampleType n, SampleType N, SampleType alpha) {
        // 0.5 * (1 - cos(2pin/alphaN)), 0 <= n <= aN / 2
        // 1, aN/2 <= n <= N / 2
        // w[N - n] = w[n], 0 <= n <= N / 2
        const auto alphaN = alpha * N;
        const auto NOverTwo = N / static_cast<SampleType>(2.0);
        const auto aNOverTwo = alphaN / static_cast<SampleType>(2.0);
        if (n >= 0 && n <= aNOverTwo) {
            // 0.5 * (1 - cos(2pin / alphaN))
            constexpr static auto twoPi = static_cast<SampleType>(2.0) * std::numbers::pi_v<SampleType>;
            const auto cosine = std::cos((twoPi * n) / alphaN);
            const auto res = static_cast<SampleType>(0.5) * (static_cast<SampleType>(1.0 - cosine));
            return res;
        } else if (n > aNOverTwo && n <= NOverTwo) {
            return static_cast<SampleType>(1.0);
        } else {
            const auto reverseIndex = N - n;
            return tukey(reverseIndex, N, alpha);
        }
    }
} // namespace marvin::math::windows
#endif