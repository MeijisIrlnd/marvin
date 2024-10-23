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
#include <marvin/math/marvin_Math.h>
#include <marvin/library/marvin_Concepts.h>
#include <numbers>
#include <cmath>
namespace marvin::math::windows {

    /**
        Represents a type of window function, for use in classes and functions which take the window type as a template argument.
    */
    enum class WindowType {
        Sine,
        Tukey,
        BlackmanHarris,
        CosineSum,
        Hann,
        Hamming
    };

    /**
     * \brief Helper class for cacheing a window function in a lookup table.
     */
    template <marvin::FloatType SampleType, size_t NumPoints>
    class PrecomputedWindow final {
    public:
        /**
         * Constructs a PrecomputedWindow, from a preallocated array of points.
         * \param lut A precomputed array containing the window function you want to store.
         */
        explicit PrecomputedWindow(std::array<SampleType, NumPoints>&& lut) : m_lut(std::move(lut)) {
        }

        /**
         * Retrieves a sample a certain (0 to 1) proportion into the LUT. Will use linear interpolation between the existing points to achieve this.
         * \param proportion A 0 to 1 propertion into the LUT.
         * \return The (interpolated) sample in the window at the given proportion.
         */
        [[nodiscard]] SampleType operator()(SampleType proportion) const {
            const auto rescaled = proportion * NumPoints;
            const auto truncated = std::trunc(rescaled);
            const auto delta = rescaled - truncated;
            const auto index0{ static_cast<size_t>(truncated) };
            const auto index1 = index0 + 1;
            const auto pointA = m_lut[index0];
            const auto pointB = m_lut[index1];
            const auto interpolated = marvin::math::lerp(pointA, pointB, delta);
            return interpolated;
        }

    private:
        std::array<SampleType, NumPoints> m_lut;
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

    /**
        A Blackman-Harris window. Very similar to a hamming / hann window, but reduces sidelobes at the cost of extra sinc terms.
        \param n The current index into the window.
        \param N The total number of points in the window. `n` will be normalised by this.
        \return A point `n/N` of the way along the Blackman-Harris window.
     */
    template <FloatType SampleType>
    [[nodiscard]] SampleType blackmanHarris(SampleType n, SampleType N) {
        constexpr static auto a0{ 0.35875 };
        constexpr static auto a1{ 0.48829 };
        constexpr static auto a2{ 0.14128 };
        constexpr static auto a3{ 0.01168 };
        constexpr static auto pi{ std::numbers::pi_v<double> };
        constexpr static auto twoPi{ pi * 2.0 };
        constexpr static auto fourPi{ pi * 4.0 };
        constexpr static auto sixPi{ pi * 6.0 };
        const auto position = static_cast<double>(n) / static_cast<double>(N);
        const auto a1Term = a1 * std::cos(position * twoPi);
        const auto a2Term = a2 * std::cos(position * fourPi);
        const auto a3Term = a3 * std::cos(position * sixPi);
        const auto result = a0 - a1Term + a2Term - a3Term;
        return static_cast<SampleType>(result);
    }

    /**
        Generalised form of cosine sum family windows, used by both hann and hamming.\n
        Implemented as `w[n] = a - (1-a) cos(2pi n/N)`.\n
        As the alpha parameter affects the boundaries of the window, you're probably better of using `hann` or `hamming` instead.
        \param n The current index into the window.
        \param N The total number of points in the window. `n` will be normalised by this.
        \param alpha [Affects the level of the start and end points](https://www.desmos.com/calculator/u3q0adsogp) - a value of 0.5 will set w[0] and w[1] to 0, anything below 0.5 will drop the start and end points below 0, and anything above 0.5 will raise the start and end points above 0.
        \return A point `n/N` of the way along the window.

    */
    template <FloatType SampleType>
    [[nodiscard]] SampleType cosineSum(SampleType n, SampleType N, SampleType alpha) {
        // w[n] = a0 - (1 - a0) * cos(2pi * n/N)
        const auto a0 = alpha;
        const auto a1 = static_cast<SampleType>(1.0) - alpha;
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto ratio = n / N;
        const auto cosine = std::cos(twoPi * ratio);
        const auto a1Scaled = a1 * cosine;
        const auto res = a0 - a1Scaled;
        return res;
    }

    /**
        A specific case of a cosine sum window with an alpha of 0.5, which goes to 0 at its boundaries, with about -18dB/8ve sidelobe rolloff in the frequency domain.
        \param n The current index into the window.
        \param N The total number of points in the window. `n` will be normalised by this.
        \return A point `n/N` of the way along the window.
     */
    template <FloatType SampleType>
    [[nodiscard]] SampleType hann(SampleType n, SampleType N) {
        return cosineSum(n, N, static_cast<SampleType>(0.5));
    }

    /**
        A specific case of a cosine sum window with an alpha of 25/46. Does *not* go to zero, but cancels the first sidelobe.
        \param n The current index into the window.
        \param N The total number of points in the window. `n` will be normalised by this.
        \return A point `n/N` of the way along the window.
     */
    template <FloatType SampleType>
    [[nodiscard]] SampleType hamming(SampleType n, SampleType N) {
        constexpr static auto alpha = static_cast<SampleType>(25.0 / 46.0);
        return cosineSum(n, N, alpha);
    }
} // namespace marvin::math::windows
#endif