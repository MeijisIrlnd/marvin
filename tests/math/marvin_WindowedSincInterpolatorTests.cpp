// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/math/marvin_VecOps.h"
#include "marvin/utils/marvin_Utils.h"
#include <cstddef>
#include <marvin/math/marvin_WindowedSincInterpolator.h>
#include <marvin/dsp/oscillators/marvin_Oscillator.h>
#include <AudioFile.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <random>
#include <filesystem>
namespace marvin::testing {

    static std::random_device s_rd;

    void resampleFile(std::string&& source, std::string&& dest, double newSampleRate) {
        AudioFile<float> loadedFile{};
        AudioFile<float> destFile{};
        destFile.setSampleRate(static_cast<std::uint32_t>(newSampleRate));
        loadedFile.load(source);
        const auto originalSampleRate = loadedFile.getSampleRate();
        const auto resamplingRatio = newSampleRate / static_cast<double>(originalSampleRate);
        math::interpolators::WindowedSincInterpolator<float, 8> interpolator{};
        // And what do we increment by?
        // 1 / resamplingRatio I guess?
        const auto newLength = static_cast<size_t>(loadedFile.getLengthInSeconds() * static_cast<double>(newSampleRate));
        destFile.setAudioBufferSize(loadedFile.getNumChannels(), static_cast<int>(newLength));
        const auto increment = 1.0 / resamplingRatio;
        for (auto channel = 0; channel < loadedFile.getNumChannels(); ++channel) {
            auto pos{ 0.0 };
            auto prevPos{ 0.0 };
            std::vector<float> resamplingBlock(8, 0.0f);
            // back of resampling block should be the first sample of the stream..
            resamplingBlock[resamplingBlock.size() - 1] = loadedFile.samples[channel][0];
            for (auto sample = 0_sz; sample < newLength; ++sample) {
                auto delta = static_cast<int>(pos) - static_cast<int>(prevPos);
                if (delta >= 1) {
                    std::rotate(resamplingBlock.begin(), resamplingBlock.begin() + 1, resamplingBlock.end());
                    // sample at the back should be the next one from the audio file..
                    resamplingBlock[resamplingBlock.size() - 1] = loadedFile.samples[channel][static_cast<size_t>(pos)];
                }
                prevPos = pos;
                const auto ratio = pos - std::floor(pos);
                const auto interpolated = interpolator.interpolate(resamplingBlock, static_cast<float>(ratio));
                destFile.samples[channel][sample] = interpolated;
                pos += increment;
            }
        }
        destFile.save(dest);
    }

    template <FloatType T, size_t N>
    std::vector<T> generateInterpolationImpulse(size_t leadingZeroes) {
        std::vector<T> vec(N, static_cast<T>(0.0));
        const auto oneIt = vec.begin() + static_cast<std::ptrdiff_t>(leadingZeroes);
        *oneIt = static_cast<T>(1.0);
        return vec;
    }

    template <FloatType T, size_t N>
    std::vector<T> generateInterpolationNoise(size_t leadingZeroes) {
        marvin::dsp::oscillators::NoiseOscillator<T> osc{ s_rd };
        std::vector<T> vec(N, static_cast<T>(0.0));
        auto startPt = vec.begin() + static_cast<std::ptrdiff_t>(leadingZeroes);
        for (auto it = startPt; it != vec.end(); ++it) {
            *it = osc();
        }
        return vec;
    }

    template <FloatType T, size_t Width, size_t OversamplingFactor>
    void testWindowedSincInterpolatorWithImpulse() {
        std::vector<T> res{};
        math::interpolators::WindowedSincInterpolator<T, Width> interpolator{};
        // We want Width leading zeroes, and then a 1, and then Width - 1 zeroes
        // Width = 3, because we need enough space at the end to have points to interpolate to.
        constexpr static auto N = Width * 3;
        auto impulse = generateInterpolationImpulse<T, N>(Width - 1);
        auto begin = impulse.begin();
        auto* beginPtr = &(*begin);
        static constexpr auto increment = static_cast<T>(1.0) / static_cast<T>(OversamplingFactor);
        const auto expectedPulsePosition = (Width / 2) * OversamplingFactor;
        for (auto i = 0; i < Width * 2; ++i) {
            auto startPtr = beginPtr + static_cast<std::ptrdiff_t>(i);
            std::span<T> view{ startPtr, Width };
            auto ratio{ static_cast<T>(0.0) };
            for (auto j = 0_sz; j < OversamplingFactor; ++j) {
                const auto interpolated = interpolator.interpolate(view, ratio);
                res.emplace_back(interpolated);
                ratio += increment;
            }
        }
        REQUIRE_THAT(res[expectedPulsePosition], Catch::Matchers::WithinRel(static_cast<T>(1.0)));
    }

    template <FloatType T, size_t Width, size_t OversamplingFactor>
    void testLinearity() {
        // Generate two random signals..
        constexpr static auto N = Width * 4;
        constexpr static auto increment = static_cast<T>(1.0) / static_cast<T>(OversamplingFactor);
        auto noiseA = generateInterpolationNoise<T, N>(Width - 1);
        auto noiseB = generateInterpolationNoise<T, N>(Width - 1);
        std::vector<T> summed(noiseA.size());
        std::vector<T> summedInterpolated{};
        std::memcpy(summed.data(), noiseA.data(), sizeof(T) * noiseA.size());
        math::vecops::add(summed, noiseB);
        // Now - sinc interpolate both vectors..
        math::interpolators::WindowedSincInterpolator<T, Width> interpolator{};
        auto beginA = noiseA.begin();
        auto* beginPtrA = &(*beginA);
        auto beginB = noiseB.begin();
        auto* beginPtrB = &(*beginB);
        auto sumBegin = summed.begin();
        auto* beginPtrSum = &(*sumBegin);
        for (auto i = 0; i < Width * 2; ++i) {
            auto startPtrA = beginPtrA + static_cast<std::ptrdiff_t>(i);
            auto startPtrB = beginPtrB + static_cast<std::ptrdiff_t>(i);
            auto startPtrSum = beginPtrSum + static_cast<std::ptrdiff_t>(i);
            std::span<T> viewA{ startPtrA, Width };
            std::span<T> viewB{ startPtrB, Width };
            std::span<T> sumView{ startPtrSum, Width };
            for (auto j = static_cast<T>(0.0); j < static_cast<T>(1.0); j += increment) {
                const auto interpolatedA = interpolator.interpolate(viewA, j);
                const auto interpolatedB = interpolator.interpolate(viewB, j);
                const auto interpolatedSummed = interpolator.interpolate(sumView, j);
                REQUIRE_THAT(interpolatedA + interpolatedB, Catch::Matchers::WithinRel(interpolatedSummed, static_cast<T>(5e-3)));
            }
        }
    }

    TEST_CASE("Test WindowedSincInterpolator") {
#if defined(MARVIN_ANALYSIS)
        const auto exePathOpt = utils::getCurrentExecutablePath();
        REQUIRE(exePathOpt);
        const auto& exePath = *exePathOpt;
        std::filesystem::path asFsPath{ exePath };
        std::filesystem::path parent = asFsPath.parent_path();
        auto sourceWav = parent;
        sourceWav += "/break.wav";
        auto destWav = parent;
        destWav += "/break_resampled.wav";
        resampleFile(sourceWav.string(), destWav.string(), 96000.0);
#endif
        testWindowedSincInterpolatorWithImpulse<float, 8, 2>();
        testWindowedSincInterpolatorWithImpulse<float, 4, 10>();
        testWindowedSincInterpolatorWithImpulse<float, 6, 10>();
        testWindowedSincInterpolatorWithImpulse<float, 8, 10>();
        testWindowedSincInterpolatorWithImpulse<float, 16, 10>();
        testWindowedSincInterpolatorWithImpulse<float, 32, 10>();
        testLinearity<float, 4, 10>();
        testLinearity<float, 4, 20>();
        testLinearity<float, 4, 2>();
        testLinearity<float, 4, 90>();
        testLinearity<float, 8, 10>();
        testLinearity<float, 8, 20>();
        testLinearity<float, 8, 2>();
        testLinearity<float, 8, 90>();
        testLinearity<float, 12, 10>();
        testLinearity<float, 12, 20>();
        testLinearity<float, 12, 2>();
        testLinearity<float, 12, 90>();
        testLinearity<float, 32, 10>();
        testLinearity<float, 32, 20>();
        testLinearity<float, 32, 2>();
        testLinearity<float, 32, 90>();

        testWindowedSincInterpolatorWithImpulse<double, 4, 10>();
        testWindowedSincInterpolatorWithImpulse<double, 6, 10>();
        testWindowedSincInterpolatorWithImpulse<double, 8, 10>();
        testWindowedSincInterpolatorWithImpulse<double, 16, 10>();
        testWindowedSincInterpolatorWithImpulse<double, 32, 10>();
        testLinearity<double, 4, 10>();
        testLinearity<double, 4, 20>();
        testLinearity<double, 4, 2>();
        testLinearity<double, 4, 90>();
        testLinearity<double, 8, 10>();
        testLinearity<double, 8, 20>();
        testLinearity<double, 8, 2>();
        testLinearity<double, 8, 90>();
        testLinearity<double, 12, 10>();
        testLinearity<double, 12, 20>();
        testLinearity<double, 12, 2>();
        testLinearity<double, 12, 90>();
        testLinearity<double, 32, 10>();
        testLinearity<double, 32, 20>();
        testLinearity<double, 32, 2>();
        testLinearity<double, 32, 90>();
    }
} // namespace marvin::testing