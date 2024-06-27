// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "catch2/matchers/catch_matchers.hpp"
#include "marvin/containers/marvin_StrideView.h"
#include "marvin/containers/marvin_SwapBuffer.h"
#include "marvin/library/marvin_Concepts.h"
#include "marvin/utils/marvin_Utils.h"
#include <cmath>
#include <limits>
#include <marvin/dsp/spectral/marvin_FFT.h>
#include <marvin/dsp/oscillators/marvin_Oscillator.h>
#include <marvin/library/marvin_Literals.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <unordered_map>
#include <array>
namespace marvin::testing {
    static std::random_device s_rd{};

    // These are all 2dB above the actual performance outputs from the IPP FFT on floats - we're checking regression on accuracy.
    static std::unordered_map<size_t, double> s_acceptablePerformances{
        { 3, -130.0 },
        { 4, -126.0 },
        { 5, -123.0 },
        { 6, -120.0 },
        { 7, -117.0 },
        { 8, -114.0 },
        { 9, -111.0 },
        { 10, -108.0 },
        { 11, -105.0 },
        { 12, -102.0 },
        { 13, -99.0 }
    };
    static std::unordered_map<size_t, double> s_errorCeilings{
        { 3, -125.0 },
        { 4, -122.0 },
        { 5, -118.0 },
        { 6, -114.0 },
        { 7, -111.0 },
        { 8, -108.0 },
        { 9, -104.0 },
        { 10, -101.0 },
        { 11, -98.0 },
        { 12, -95.0 },
        { 13, -92.0 }

    };

    template <FloatType SampleType, size_t N>
    void generateComplexSinusoid(size_t n, std::span<std::complex<SampleType>> res) {
        constexpr static auto twoPi = static_cast<SampleType>(2.0) * std::numbers::pi_v<SampleType>;
        for (auto i = 0_sz; i < N; ++i) {
            const auto phase = twoPi * static_cast<SampleType>(n) * (static_cast<SampleType>(i) / static_cast<SampleType>(N));
            res[i] = { std::cos(phase), std::sin(phase) };
        }
    }

    template <FloatType SampleType, size_t Order>
    void testComplexSinusoidResponse() {
        auto largestError{ static_cast<SampleType>(0.0) };
        constexpr static auto size = 1 << Order;
        dsp::spectral::FFT<std::complex<SampleType>> engine{ Order };
        std::array<std::complex<SampleType>, size> sinusoid;
        auto rmsPerformance = static_cast<SampleType>(0.0);
        auto averageErrorDb = static_cast<SampleType>(0.0);
        for (auto i = 0_sz; i < engine.getFFTSize(); ++i) {
            generateComplexSinusoid<SampleType, size>(i, sinusoid);
            auto res = engine.forward(sinusoid);
            auto rmsError = static_cast<SampleType>(0.0);
            auto rmsExpected = static_cast<SampleType>(0.0);
            for (auto j = 0_sz; j < size; ++j) {
                const std::complex<SampleType> expected = j == i ? static_cast<SampleType>(1.0) : static_cast<SampleType>(0.0);
                if (j == i) {
                    REQUIRE_THAT(res[j].real(), Catch::Matchers::WithinRel(expected.real()));
                }
                const auto actual = res[j];
                const auto delta = std::abs(actual - expected);
                if (largestError < delta) {
                    largestError = delta;
                }
                const auto deltaSquared = std::norm(actual - expected);
                rmsError += deltaSquared;
                rmsExpected += std::norm(expected);
            }
            rmsError /= static_cast<SampleType>(size);
            rmsError = std::sqrt(rmsError);
            if (rmsError == 0) rmsError = std::numeric_limits<SampleType>::epsilon();
            const auto errorDb = static_cast<SampleType>(20) * std::log10(rmsError / std::sqrt(static_cast<SampleType>(size)));
            averageErrorDb += errorDb;
            rmsExpected /= static_cast<SampleType>(size);
            rmsExpected = std::sqrt(rmsExpected);
            const auto performance = rmsError / rmsExpected;
            rmsPerformance += std::norm(performance);
        }
        averageErrorDb /= static_cast<SampleType>(size);
        rmsPerformance /= static_cast<SampleType>(size);
        rmsPerformance = std::sqrt(rmsPerformance);
        auto performanceDb = static_cast<SampleType>(20) * std::log10(rmsPerformance / std::sqrt(static_cast<SampleType>(size)));
        REQUIRE(performanceDb < s_acceptablePerformances[Order]);
    }

    template <FloatType SampleType, size_t Order>
    void testComplexSinusoidRoundTrip() {
        constexpr static auto Size = 1 << Order;
        dsp::spectral::FFT<std::complex<SampleType>> engine{ Order };
        std::array<std::complex<SampleType>, Size> sinusoid;
        std::array<std::complex<SampleType>, Size> freqDomainResults, timeDomainResults;
        const auto acceptablePeakErrorDb = s_errorCeilings[Order];
        for (auto i = 0_sz; i < Size; ++i) {
            generateComplexSinusoid<SampleType, Size>(i, sinusoid);
            engine.forward(sinusoid, freqDomainResults);
            engine.inverse(freqDomainResults, timeDomainResults);
            for (auto j = 0_sz; j < Size; ++j) {
                auto deltaMagnitude = std::sqrt(std::norm(timeDomainResults[j] - sinusoid[j]));
                deltaMagnitude = deltaMagnitude == 0 ? std::numeric_limits<SampleType>::epsilon() : deltaMagnitude;
                const auto deltaDb = static_cast<SampleType>(20.0) * std::log10(deltaMagnitude);
                REQUIRE(deltaDb < acceptablePeakErrorDb);
            }
        }
    }

    template <FloatType SampleType, size_t Order>
    void testLinearity() {
        constexpr static auto Size = 1 << Order;
        marvin::dsp::oscillators::NoiseOscillator<SampleType> noiseOsc{ s_rd };
        std::array<SampleType, Size> randInputA, randInputB, summedInput;
        std::array<std::complex<SampleType>, (Size / 2) + 1> resultsA, resultsB, summedResults;
        for (auto i = 0_sz; i < Size; ++i) {
            randInputA[i] = noiseOsc();
            randInputB[i] = noiseOsc();
            summedInput[i] = randInputA[i] + randInputB[i];
        }
        dsp::spectral::FFT<SampleType> engine{ Order };
        engine.forward(randInputA, resultsA);
        engine.forward(randInputB, resultsB);
        engine.forward(summedInput, summedResults);
        const auto acceptableError = s_errorCeilings[Order];
        for (auto i = 0_sz; i < resultsA.size(); ++i) {
            const auto sum = resultsA[i] + resultsB[i];
            const auto delta = std::sqrt(std::norm(sum - summedResults[i]));
            const auto deltaDb = static_cast<SampleType>(20.0) * std::log10(delta);
            REQUIRE(deltaDb < acceptableError);
        }
    }
    template <FloatType SampleType, size_t Order>
    void testRealOnlyRoundTrip() {
        constexpr static auto Size = 1 << Order;
        const auto acceptableError = s_errorCeilings[Order];
        marvin::dsp::oscillators::NoiseOscillator<SampleType> noiseOsc{ s_rd };
        std::array<SampleType, Size> signal;
        std::array<SampleType, Size> results;
        for (auto i = 0_sz; i < Size; ++i) {
            signal[i] = noiseOsc();
        }
        dsp::spectral::FFT<SampleType> engine{ Order };
        auto freqDomain = engine.forward(signal);
        engine.inverse(freqDomain, results);
        for (auto i = 0_sz; i < Size; ++i) {
            const auto delta = std::sqrt(std::norm(results[i] - signal[i]));
            const auto deltaDb = static_cast<SampleType>(20.0) * std::log10(delta);
            REQUIRE(deltaDb < acceptableError);
        }
    }

    TEST_CASE("Test Real-Only Round Trip") {
        testRealOnlyRoundTrip<float, 3>();
        testRealOnlyRoundTrip<float, 4>();
        testRealOnlyRoundTrip<float, 5>();
        testRealOnlyRoundTrip<float, 6>();
        testRealOnlyRoundTrip<float, 7>();
        testRealOnlyRoundTrip<float, 8>();
        testRealOnlyRoundTrip<float, 9>();
        testRealOnlyRoundTrip<float, 10>();
        testRealOnlyRoundTrip<float, 11>();
        // testRealOnlyRoundTrip<float, 12>();
        // testRealOnlyRoundTrip<float, 13>();
        testRealOnlyRoundTrip<double, 3>();
        testRealOnlyRoundTrip<double, 4>();
        testRealOnlyRoundTrip<double, 5>();
        testRealOnlyRoundTrip<double, 6>();
        testRealOnlyRoundTrip<double, 7>();
        testRealOnlyRoundTrip<double, 8>();
        testRealOnlyRoundTrip<double, 9>();
        testRealOnlyRoundTrip<double, 10>();
        testRealOnlyRoundTrip<double, 11>();
        // testRealOnlyRoundTrip<double, 12>();
        // testRealOnlyRoundTrip<double, 13>();
    }

    TEST_CASE("Test Complex Sinusoidal Input") {
        testComplexSinusoidRoundTrip<float, 3>();
        testComplexSinusoidRoundTrip<float, 4>();
        testComplexSinusoidRoundTrip<float, 5>();
        testComplexSinusoidRoundTrip<float, 6>();
        testComplexSinusoidRoundTrip<float, 7>();
        testComplexSinusoidRoundTrip<float, 8>();
        testComplexSinusoidRoundTrip<float, 9>();
        testComplexSinusoidRoundTrip<float, 10>();
        testComplexSinusoidRoundTrip<float, 11>();
        // testComplexSinusoidRoundTrip<float, 12>();
        // testComplexSinusoidRoundTrip<float, 13>();
        testComplexSinusoidRoundTrip<double, 3>();
        testComplexSinusoidRoundTrip<double, 4>();
        testComplexSinusoidRoundTrip<double, 5>();
        testComplexSinusoidRoundTrip<double, 6>();
        testComplexSinusoidRoundTrip<double, 7>();
        testComplexSinusoidRoundTrip<double, 8>();
        testComplexSinusoidRoundTrip<double, 9>();
        testComplexSinusoidRoundTrip<double, 10>();
        testComplexSinusoidRoundTrip<double, 11>();
        // testComplexSinusoidRoundTrip<double, 12>();
        // testComplexSinusoidRoundTrip<double, 13>();
        testComplexSinusoidResponse<float, 3>();
        testComplexSinusoidResponse<float, 4>();
        testComplexSinusoidResponse<float, 5>();
        testComplexSinusoidResponse<float, 6>();
        testComplexSinusoidResponse<float, 7>();
        testComplexSinusoidResponse<float, 8>();
        testComplexSinusoidResponse<float, 9>();
        testComplexSinusoidResponse<float, 10>();
        testComplexSinusoidResponse<float, 11>();
        // testComplexSinusoidResponse<float, 12>();
        // testComplexSinusoidResponse<float, 13>();
        testComplexSinusoidResponse<double, 3>();
        testComplexSinusoidResponse<double, 4>();
        testComplexSinusoidResponse<double, 5>();
        testComplexSinusoidResponse<double, 6>();
        testComplexSinusoidResponse<double, 7>();
        testComplexSinusoidResponse<double, 8>();
        testComplexSinusoidResponse<double, 9>();
        testComplexSinusoidResponse<double, 10>();
        testComplexSinusoidResponse<double, 11>();
        // testComplexSinusoidResponse<double, 12>();
        // testComplexSinusoidResponse<double, 13>();
    }

    TEST_CASE("Test Linearity") {
        testLinearity<float, 3>();
        testLinearity<float, 4>();
        testLinearity<float, 5>();
        testLinearity<float, 6>();
        testLinearity<float, 7>();
        testLinearity<float, 8>();
        testLinearity<float, 9>();
        testLinearity<float, 10>();
        testLinearity<float, 11>();
        // testLinearity<float, 12>();
        // testLinearity<float, 13>();
        testLinearity<double, 3>();
        testLinearity<double, 4>();
        testLinearity<double, 5>();
        testLinearity<double, 6>();
        testLinearity<double, 7>();
        testLinearity<double, 8>();
        testLinearity<double, 9>();
        testLinearity<double, 10>();
        testLinearity<double, 11>();
        // testLinearity<double, 12>();
        // testLinearity<double, 13>();
    }

} // namespace marvin::testing