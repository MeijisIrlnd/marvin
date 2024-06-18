// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/containers/marvin_StrideView.h"
#include <marvin/dsp/spectral/marvin_FFT.h>
#include <marvin/dsp/oscillators/marvin_Oscillator.h>
#include <marvin/library/marvin_Literals.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <vector>
#include <iostream>
namespace marvin::testing {
    static std::random_device rd{};
    static int s_iterCount{ 0 };

    template <FloatType SampleType, size_t N, size_t Stride, size_t StartOffset = 0>
    void butterfly(marvin::containers::StrideView<SampleType, Stride> x, std::span<std::complex<SampleType>> results, bool even = true) {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        if constexpr (N == 1) {
            return;
        } else {
            // generate a new stride view..
            constexpr auto nextN = N / 2;
            constexpr auto nextStride = Stride * 2;
            marvin::containers::StrideView<SampleType, nextStride> evenView{ x.underlying().data(), x.underlying().size() };
            auto offsetIt = x.begin() + static_cast<std::ptrdiff_t>(1);
            auto* offsetPtr = &(*offsetIt);
            marvin::containers::StrideView<SampleType, nextStride> oddView{ offsetPtr, x.underlying().size() };
            butterfly<SampleType, nextN, nextStride, StartOffset>(evenView, results);
            butterfly<SampleType, nextN, nextStride, StartOffset + Stride>(oddView, results, false);
            std::cout << "==========================\n";
            for (auto i = 0_sz; i < evenView.size(); ++i) {
                const auto x_even = evenView[i];
                const auto x_odd = oddView[i];
                std::cout << "Pairing " << x_even << " with " << x_odd << "\n";
                std::cout << "Start Offset + Stride " << StartOffset + (i * Stride) << "\n";
                const auto twiddleFrac = static_cast<SampleType>(i) / static_cast<SampleType>(N);
                const auto exponent = -1 * twoPi * twiddleFrac;
                std::complex<SampleType> tf{ std::cos(exponent), std::sin(exponent) };
                const auto res = x_even + (tf * x_odd);
            }
        }
    }
    template <FloatType T>
    std::vector<T> generateNoise(size_t N) {
        std::vector<T> vec;
        marvin::dsp::oscillators::NoiseOscillator<T> osc{ rd };
        for (auto i = 0_sz; i < N; ++i) {
            vec.emplace_back(osc());
        }
        return vec;
    }

    template <FloatType T, size_t Order>
    void testFFT() {
        marvin::dsp::spectral::FFT<T> fft{ Order };
        const auto expectedSize = static_cast<size_t>(std::pow(2, Order));
#if defined(MARVIN_MACOS)
        const auto expectedEngine = marvin::dsp::spectral::EngineType::Accelerate_FFT;
#else
#if defined(MARVIN_HAS_IPP)
        const auto expectedEngine = marvin::dsp::spectral::EngineType::Ipp_FFT;
#else
        const auto expectedEngine = marvin::dsp::spectral::EngineType::Fallback_FFT;
#endif
#endif
        REQUIRE(fft.getEngineType() == expectedEngine);
        REQUIRE(fft.getFFTSize() == expectedSize);
        {
            auto impulse = generateNoise<T>(expectedSize);
            auto fftData = fft.performForwardTransform(impulse);
            // CCS has DC[0] and Nyquist at N, both of which have 0 imags (they're on the real axis).
            REQUIRE_THAT(fftData.front().imag(), Catch::Matchers::WithinRel(static_cast<T>(0.0)));
            REQUIRE_THAT(fftData[fftData.size() - 1].imag(), Catch::Matchers::WithinRel(static_cast<T>(0.0)));
            for (auto& el : fftData) {
                REQUIRE_FALSE(std::isnan(el.real()));
                REQUIRE_FALSE(std::isnan(el.imag()));
                REQUIRE_FALSE(std::isinf(el.real()));
                REQUIRE_FALSE(std::isinf(el.imag()));
            }
            auto ifftData = fft.performInverseTransform(fftData);
            REQUIRE(ifftData.size() == expectedSize);
            for (size_t i = 0; i < ifftData.size(); ++i) {
                REQUIRE_THAT(ifftData[i], Catch::Matchers::WithinRel(impulse[i], static_cast<T>(0.2)));
            }
        }
        {
            auto impulse = generateNoise<T>(expectedSize);
            std::vector<std::complex<T>> destVec((expectedSize / 2) + 1);
            fft.performForwardTransform(impulse, destVec);
            REQUIRE_THAT(destVec.front().imag(), Catch::Matchers::WithinRel(static_cast<T>(0.0)));
            REQUIRE_THAT(destVec[destVec.size() - 1].imag(), Catch::Matchers::WithinRel(static_cast<T>(0.0)));
            for (auto& el : destVec) {
                REQUIRE_FALSE(std::isnan(el.real()));
                REQUIRE_FALSE(std::isnan(el.imag()));
                REQUIRE_FALSE(std::isinf(el.real()));
                REQUIRE_FALSE(std::isinf(el.imag()));
            }
            std::vector<T> ifftVec(expectedSize);
            fft.performInverseTransform(destVec, ifftVec);
            for (size_t i = 0; i < ifftVec.size(); ++i) {
                REQUIRE_THAT(ifftVec[i], Catch::Matchers::WithinRel(impulse[i], static_cast<T>(0.2)));
            }
        }
    }

    TEST_CASE("Test FFT") {
        // testFFT<float, 2>();
        // testFFT<float, 3>();
        // testFFT<float, 4>();
        // testFFT<float, 5>();
        // testFFT<float, 6>();
        // testFFT<float, 7>();
        // testFFT<float, 8>();
        // testFFT<float, 9>();
        // testFFT<float, 10>();
        // testFFT<float, 11>();
        // testFFT<float, 12>();
        // testFFT<double, 2>();
        // testFFT<double, 3>();
        // testFFT<double, 4>();
        // testFFT<double, 5>();
        // testFFT<double, 6>();
        // testFFT<double, 7>();
        // testFFT<double, 8>();
        // testFFT<double, 9>();
        // testFFT<double, 10>();
        // testFFT<double, 11>();
        // testFFT<double, 12>();
    }

    TEST_CASE("Test recurse") {
        std::vector<float> test(8, 0.0f);
        test[0] = 1.0f;
        for (auto i = 0_sz; i < 8; ++i) {
            test[i] = static_cast<float>(i);
        }
        std::vector<std::complex<float>> resVec(8, { 0.0f, 0.0f });
        butterfly<float, 8>(containers::StrideView<float, 1>{ test }, resVec);
        // std::vector<std::complex<float>> res(32);
        // dsp::spectral::recurse<float, 32, 1>(test, res);
    }

} // namespace marvin::testing