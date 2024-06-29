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
#include <iostream>
namespace marvin::testing {
    static std::random_device s_rd{};

    // These are all 2dB above the actual performance outputs from the IPP FFT on floats - we're checking regression on accuracy.
    static std::unordered_map<size_t, double> s_acceptablePerformances{
        { 3, -129.0 },
        { 4, -124.0 },
        { 5, -121.0 },
        { 6, -118.0 },
        { 7, -115.0 },
        { 8, -112.0 },
        { 9, -109.0 },
        { 10, -106.0 },
        { 11, -103.0 },
        { 12, -100.0 },
        { 13, -97.0 }
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
                const auto actual = res[j] / static_cast<SampleType>(size);
                if (j == i) {
                    REQUIRE_THAT(actual.real(), Catch::Matchers::WithinRel(expected.real()));
                }
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
        for (auto i = 0_sz; i < Size; ++i) {
            auto rmsError = static_cast<SampleType>(0.0);
            generateComplexSinusoid<SampleType, Size>(i, sinusoid);
            engine.forward(sinusoid, freqDomainResults);
            engine.inverse(freqDomainResults, timeDomainResults);
            for (auto j = 0_sz; j < Size; ++j) {
                rmsError += std::norm(timeDomainResults[j] - sinusoid[j]);
            }
            rmsError /= static_cast<SampleType>(Size);
            rmsError = std::sqrt(rmsError);
            const auto errorDb = static_cast<SampleType>(20) * std::log10(rmsError);
            REQUIRE(errorDb < s_acceptablePerformances[Order]);
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
        auto rmsError{ static_cast<SampleType>(0.0) };
        for (auto i = 0_sz; i < resultsA.size(); ++i) {
            const auto sum = resultsA[i] + resultsB[i];
            rmsError += std::norm(sum - summedResults[i]);
        }
        rmsError /= static_cast<SampleType>(Size);
        rmsError = std::sqrt(rmsError);
        const auto errorDb = static_cast<SampleType>(20.0) * std::log10(rmsError);
        REQUIRE(errorDb < s_acceptablePerformances[Order]);
    }
    template <FloatType SampleType, size_t Order>
    void testRealOnlyRoundTrip() {
        constexpr static auto Size = 1 << Order;
        auto rmsError = static_cast<SampleType>(0.0);
        marvin::dsp::oscillators::NoiseOscillator<SampleType> noiseOsc{ s_rd };
        std::array<SampleType, Size> signal;
        std::array<std::complex<SampleType>, Size> fftResults;
        std::array<SampleType, Size> results;
        for (auto i = 0_sz; i < Size; ++i) {
            signal[i] = noiseOsc();
        }
        dsp::spectral::FFT<SampleType> engine{ Order };
        auto freqDomain = engine.forward(signal);
        engine.inverse(freqDomain, results);
        for (auto i = 0_sz; i < Size; ++i) {
            rmsError += std::norm(results[i] - signal[i]);
        }
        rmsError /= static_cast<SampleType>(Size);
        rmsError = std::sqrt(rmsError);
        const auto errorDb = static_cast<SampleType>(20.0) * std::log10(rmsError);
        REQUIRE(errorDb < s_acceptablePerformances[Order]);
    }

    template <FloatType SampleType, size_t Order>
    void testRealImpulse() {
        constexpr static auto Size = 1 << Order;
        std::array<SampleType, Size> impulse, fftResults;
        std::array<std::complex<SampleType>, (Size / 2) + 1> results;
        std::fill(impulse.begin(), impulse.end(), static_cast<SampleType>(0.0));
        impulse.front() = static_cast<SampleType>(1.0);
        dsp::spectral::FFT<SampleType> engine{ Order };
        engine.forward(impulse, results);
        engine.inverse(results, fftResults);
        auto rmsErr = static_cast<SampleType>(0.0);
        for (auto i = 0_sz; i < Size; ++i) {
            rmsErr += std::norm(fftResults[i] - impulse[i]);
            // REQUIRE_THAT(fftResults[i], Catch::Matchers::WithinRel(impulse[i]));
        }
        rmsErr /= static_cast<SampleType>(Size);
        rmsErr = std::sqrt(rmsErr);
        const auto rmsErrDb = static_cast<SampleType>(20.0) * std::log10(rmsErr);
        REQUIRE(rmsErrDb < s_acceptablePerformances[Order]);
    }

    template <FloatType SampleType, size_t Order>
    void testComplexImpulse() {
        constexpr static auto Size = 1 << Order;
        std::array<std::complex<SampleType>, Size> impulse, results, fftResults;
        const std::complex<SampleType> zeroComplex{ static_cast<SampleType>(0.0), static_cast<SampleType>(0.0) };
        std::fill(impulse.begin(), impulse.end(), zeroComplex);
        impulse.front() = { static_cast<SampleType>(1.0), static_cast<SampleType>(0.0) };
        dsp::spectral::FFT<std::complex<SampleType>> engine{ Order };
        engine.forward(impulse, results);
        engine.inverse(results, fftResults);
        for (auto i = 0_sz; i < Size; ++i) {
            REQUIRE_THAT(fftResults[i].real(), Catch::Matchers::WithinRel(impulse[i].real()));
            REQUIRE_THAT(fftResults[i].imag(), Catch::Matchers::WithinRel(impulse[i].imag()));
        }
    }

    TEST_CASE("Test Real-Only Round Trip") {
        testRealImpulse<float, 4>();
        testComplexImpulse<float, 4>();
        testRealOnlyRoundTrip<float, 3>();
        testRealOnlyRoundTrip<float, 4>();
        testRealOnlyRoundTrip<float, 5>();
        testRealOnlyRoundTrip<float, 6>();
        testRealOnlyRoundTrip<float, 7>();
        testRealOnlyRoundTrip<float, 8>();
        testRealOnlyRoundTrip<float, 9>();
        testRealOnlyRoundTrip<float, 10>();
        testRealOnlyRoundTrip<float, 11>();
        testRealOnlyRoundTrip<float, 12>();
        testRealOnlyRoundTrip<float, 13>();
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