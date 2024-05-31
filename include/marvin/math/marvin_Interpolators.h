// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_INTERPOLATORS_H
#define MARVIN_INTERPOLATORS_H
#include <marvin/library/marvin_Concepts.h>
#include <marvin/library/marvin_Literals.h>
#include <marvin/math/marvin_Math.h>
#include <marvin/math/marvin_Windows.h>
#include <span>
#include <cassert>
#include <vector>
#include <functional>
namespace marvin::math::interpolators {
    /**
        \brief A windowed sinc interpolator, suitable for use in a realtime context.

        Uses a lookup table precomputed at construction time for optimization purposes, and bakes the selected window function into the lookup table.
        For a non-integer subsample, the resulting point on the Sinc curve will be a lerp between the closest two points in the table.
        Input should contain `N - 1` `history` samples, followed by the latest sample. For example, in the case of `N=4`,
        The input should be in the form `[x[n-3], x[n-2], x[n-1], x[n]]`. <br>In the case of `N=8`, it should be in the form
        `[x[n-7], x[n-6], x[n-5], x[n-4], x[n-3], x[n-2], x[n-1], x[n]]`.<br>
        The interpolator will introduce `N / 2` samples of latency (determined by assuming a fixed ratio of `0`,
        and counting the number of samples it takes an impulse to be output). For example, a signal
        `[a, b, c, d, e, f, g, .......]`, with `N=4` would be output as
        `[0, 0, a, b, c, d ......]`. <br>
        To really belabour the point, the inputs passed to the interpolator for the above impulse and `N=4` would be:

        ```
            [0, 0, 0, a] => 0 // we have no history, so use zeroes.
            [0, 0, a, b] => 0
            [0, a, b, c] => a
            [a, b, c, d] => b
            [b, c, d, e] => c
            [c, d, e, f] => d
            ....
        ```

        Usage Example:
        ```cpp
        template<size_t N>
        void upsample(std::span<float> source, double sourceSampleRate, std::vector<float>& dest, double desiredSampleRate) {
            using namespace marvin::literals;
            const auto originalLengthSeconds = static_cast<double>(source.size()) / sourceSampleRate;
            const auto destSize = static_cast<size_t>(originalLengthSeconds * desiredSampleRate);
            dest.resize(destSize);
            const auto resamplingRatio = desiredSampleRate / sourceSampleRate;
            const auto increment = 1.0 / resamplingRatio;
            std::vector<float> resamplingBlock(N, 0.0f);
            resamplingBlock[resamplingBlock.size()] = source[0]; // The back of resampling block == the current sample..
            marvin::math::WindowedSincInterpolator<float, N> interpolator{};
            auto pos{ 0.0 }, prevPos{ 0.0 };
            for(auto i = 0_sz; i < destSize; ++i) {
                auto truncatedDelta = static_cast<int>(pos) - static_cast<int>(prevPos);
                if(truncatedDelta >= 1) { // Note that if the sample rates are significantly different (ie 44100 -> 22050), then truncated delta could potentially be >= 2. In that case, the condition will need to account for multiple samples being added and removed from the resampling block at once, rather than assuming a single sample delta. This could be achieved in a while loop, for example, but is left out for the sake of brevity, and is why the function is called "upsample" instead of "resample".
                    std::rotate(resamplingBlock.begin(), resamplingBlock.begin() + 1, resamplingBlock.end()); // [a, b, c, d] -> [b, c, d, a]
                    const auto next = source[static_cast<size_t>(pos)];
                    resamplingBlock[resamplingBlock.size() - 1] = next; // [b, c, d, a] -> [b, c, d, e]
                }
                prevPos = pos;
                const auto ratio = pos - std::floor(pos);
                const auto resampled = interpolator.interpolate(resamplingBlock, ratio);
                dest[i] = resampled;
                pos += increment;
            }
        }
        ```
    */
    template <FloatType SampleType, size_t N, windows::WindowType WindowType>
    requires(N % 2 == 0)
    class WindowedSincInterpolator {
    public:
        /**
            Constructs a WindowedSincInterpolator with a sine window applied to the lookup table.
        */
        WindowedSincInterpolator()
        requires(WindowType == windows::WindowType::Sine)
        {
            auto windowFunc = [](SampleType i) -> SampleType {
                const auto res = windows::sine(i, static_cast<SampleType>(N));
                return res;
            };
            fillLookupTable(std::move(windowFunc));
        }

        /**
            Constructs a WindowedSincInterpolator with a tukey window applied to the lookup table.
            \param alpha At 0, the window is rectangular, and at 1, the window is a hann window.
        */
        explicit WindowedSincInterpolator(SampleType alpha)
        requires(WindowType == windows::WindowType::Tukey)
        {
            auto windowFunc = [alpha](SampleType i) -> SampleType {
                const auto res = windows::tukey(i, static_cast<SampleType>(N), alpha);
                return res;
            };
            fillLookupTable(std::move(windowFunc));
        }

        /**
            Performs the interpolation. See the detailed description for an explanation of the formatting needed for `sampleContext`.
            \param sampleContext An array like containing `N-1` previous samples, followed by the current sample. <b>Must</b> be the same size as `N`.
            \param ratio The 0 to 1 subsample position of the interpolation. For an input block `[a b c d]`, when ratio = 0, the output will be `b`. When ratio = 1, the output will be `c`.
            When ratio = 0.5, the output will be the sinc interpolated midpoint between `b` and `c`, which is left to the reader's imagination.
            \return The next interpolated sample.

        */
        [[nodiscard]] SampleType interpolate(std::span<SampleType> sampleContext, SampleType ratio) {
            assert(sampleContext.size() == N);
            auto sum = static_cast<SampleType>(0.0);
            const auto inverseRatio = static_cast<SampleType>(1.0) - ratio;
            constexpr static auto halfN = static_cast<int>(N) / 2;
            for (auto i = 0_sz; i < N; ++i) {
                auto n = static_cast<int>(i) - static_cast<int>(halfN);
                const auto offsetN = static_cast<SampleType>(n) + inverseRatio;
                const auto windowedSinc = lookupSinc(n, inverseRatio);
                const auto res = sampleContext[i] * windowedSinc;
                sum += res;
            }
            // for (auto n = -halfN; n < halfN; ++n) {
            //     const auto actualIndex = static_cast<size_t>(n + halfN);
            //     const auto windowedSinc = lookupSinc(n + inverseRatio);
            //     const auto res = sampleContext[actualIndex] * windowedSinc;
            //     sum += res;
            // }
            return sum;
        }

        // [[nodiscard]] SampleType interpolate(std::span<SampleType> sampleContext, SampleType ratio) {
        //     assert(sampleContext.size() == N);
        //     auto sum = static_cast<SampleType>(0.0);
        //     // Multiply all elements in sampleContext by our sinc window..
        //     for (auto i = 0_sz; i < N; ++i) {
        //         const auto n = static_cast<int>(i) - static_cast<int>(N / 2);
        //         const auto offsetN{ static_cast<SampleType>(n) + (static_cast<SampleType>(1.0) - ratio) };
        //         // const auto window = windows::tukey(offsetN, static_cast<SampleType>(N), static_cast<SampleType>(1.0));
        //         const auto window = windows::tukey(static_cast<SampleType>(i), static_cast<SampleType>(N), static_cast<SampleType>(0.2));
        //         // const auto window = windows::sine(static_cast<SampleType>(n), static_cast<SampleType>(N));
        //         const auto res = math::sinc(offsetN) * window;
        //         const auto windowedSample = sampleContext[i] * res;
        //         sum += windowedSample;
        //     }
        //     return sum;
        // }

    private:
        void fillLookupTable(std::function<SampleType(SampleType)>&& windowFunction) {
            m_lut.reserve(N + 2);
            m_lut.resize(N);
            constexpr static auto halfWidth{ N / 2 };
            for (auto i = 0_sz; i < N; ++i) {
                const auto n{ static_cast<int>(i) - static_cast<int>(halfWidth) };
                const auto window = windowFunction(static_cast<SampleType>(i));
                const auto sincRes = sinc(static_cast<SampleType>(n));
                const auto windowedSinc = sincRes * window;
                m_lut[i] = windowedSinc;
            }
            const auto start = m_lut.front();
            const auto end = m_lut[m_lut.size() - 1];
            m_lut.emplace_back(end);
            m_lut.emplace_back(start);
            std::rotate(m_lut.begin(), m_lut.end() - 1, m_lut.end()); // [ x x x x e s ] -> [s x x x x e]
        }


        [[nodiscard]] SampleType lookupSinc(SampleType x, SampleType frac) {
            constexpr static auto halfN{ static_cast<int>(N / 2) };
            const auto nBelow = static_cast<int>(std::floor(x));
            const auto nAbove = static_cast<int>(nBelow) + 1;
            const auto ratio = frac;
            // const auto ratio = x - static_cast<SampleType>(nBelow);
            // So this is now in range -N to N
            // translating this to an actual index into the LUT means just adding (N / 2)..
            // LUT has an extra point at the start compared to what we actually expect....
            const auto idxBelow = static_cast<size_t>(nBelow + halfN) + 1;
            const auto idxAbove = static_cast<size_t>(nAbove + halfN) + 1;
            const auto start = m_lut[idxBelow];
            const auto end = m_lut[idxAbove];
            const auto interpolated = math::lerp<SampleType>(start, end, ratio);
            return interpolated;
        }

        std::vector<SampleType> m_lut;
    };
} // namespace marvin::math::interpolators

#endif
