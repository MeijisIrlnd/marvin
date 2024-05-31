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
namespace marvin::testing {
    static std::random_device rd{};

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
        std::vector<float> test(32, 0.0f);
        test.front() = 1.0f;
        std::vector<std::complex<float>> res(32);
        dsp::spectral::recurse<float, 32, 1>(test, res);
    }

} // namespace marvin::testing