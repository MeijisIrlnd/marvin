//
// Created by Syl Morrison on 03/05/2025.
//
#include "catch2/benchmark/catch_benchmark.hpp"


#include <iostream>
#include <marvin/dsp/filters/biquad/marvin_Biquad.h>
#include <marvin/dsp/filters/biquad/marvin_RBJCoefficients.h>
#include <marvin/dsp/filters/biquad/marvin_SIMDBiquad.h>
#include <marvin/dsp/oscillators/marvin_Oscillator.h>
#include <fmt/core.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
namespace marvin::testing {
    static std::random_device s_rd;
    template <marvin::FloatType SampleType>
    std::vector<SampleType> generateImpulse(size_t len) {
        std::vector<SampleType> impulse;
        impulse.resize(len);
        std::fill(impulse.begin(), impulse.end(), static_cast<SampleType>(0.0));
        impulse[0] = static_cast<SampleType>(1.0);
        return impulse;
    }
    template <FloatType T, size_t N>
    std::vector<T> generateNoise() {
        marvin::dsp::oscillators::NoiseOscillator<T> osc{ s_rd };
        std::vector<T> vec(N, static_cast<T>(0.0));
        for (auto i = 0; i < N; ++i) {
            vec[i] = osc();
        }
        return vec;
    }

    TEST_CASE("Test parity with single biquad") {
        constexpr static auto sampleRate{ 44100.0 };
        constexpr static auto cutoff{ 200.0 };
        constexpr static auto q{ 0.7070 };
        marvin::dsp::filters::Biquad<double, 1> singleBiquad;
        marvin::dsp::filters::SIMDBiquad<double, 1> simdBiquad;
        auto lowpassCoeffs = dsp::filters::rbj::lowpass(sampleRate, cutoff, q);
        singleBiquad.setCoeffs(0, lowpassCoeffs);
        simdBiquad.setCoeffs(lowpassCoeffs);
        auto impulse = generateImpulse<double>(100);
        for (auto i = 0; i < impulse.size(); ++i) {
            const auto singleFiltered = singleBiquad(impulse[i]);
            std::array<double, 1> simdInput{ impulse[i] };
            simdBiquad(simdInput);
            std::cout << i;
            REQUIRE_THAT(simdInput[0], Catch::Matchers::WithinRel(singleFiltered, 0.1));
        }
    }

    template <NumericType T>
    [[nodiscard]] std::string getTypeName() {
        if constexpr (std::is_same_v<T, float>) {
            return "float";
        } else if constexpr (std::is_same_v<T, double>) {
            return "double";
        }
    }

    template <marvin::FloatType SampleType, size_t N, size_t NumSamples>
    auto benchmarkSIMD() -> void {
        constexpr static auto sampleRate{ 44100.0 };
        constexpr static auto cutoff{ 200.0 };
        constexpr static auto q{ 0.7070 };
        const auto coeffs = marvin::dsp::filters::rbj::lowpass<SampleType>(sampleRate, cutoff, q);
        std::array<marvin::dsp::filters::Biquad<SampleType, 1>, N> normalFilters;
        marvin::dsp::filters::SIMDBiquad<SampleType, N> simdFilters;
        for (auto& f : normalFilters) {
            f.setCoeffs(0, coeffs);
        }
        simdFilters.setCoeffs(coeffs);
        auto impulse = generateNoise<SampleType, NumSamples>();
        std::vector<std::array<SampleType, N>> simdInputs;
        for (auto& x : impulse) {
            std::array<SampleType, N> current;
            std::fill(current.begin(), current.end(), x);
            simdInputs.emplace_back(current);
        }
        BENCHMARK(fmt::format("Biquad, N = {}, NSamples = {}, Type = {}", N, NumSamples, getTypeName<SampleType>(), N)) {
            for (auto i = 0; i < impulse.size(); ++i) {
                const auto coeffs = marvin::dsp::filters::rbj::lowpass<SampleType>(sampleRate, cutoff + static_cast<SampleType>(i), q);

                for (auto& f : normalFilters) {
                    f.setCoeffs(0, coeffs);
                    [[maybe_unused]] const auto _ = f(impulse[i]);
                }
            }
        };
        BENCHMARK(fmt::format("SIMDBiquad<{}>, NSamples = {}, Type = {}", N, NumSamples, getTypeName<SampleType>(), N)) {
            for (auto i = 0; i < impulse.size(); ++i) {
                const auto coeffs = marvin::dsp::filters::rbj::lowpass<SampleType>(sampleRate, cutoff + static_cast<SampleType>(i), q);
                simdFilters.setCoeffs(coeffs);
                simdFilters(simdInputs[i]);
            }
        };
    }

    TEST_CASE("Benchmark Biquads") {
        benchmarkSIMD<float, 2, 32>();
        benchmarkSIMD<float, 3, 32>();
        benchmarkSIMD<float, 4, 32>();
        benchmarkSIMD<float, 5, 32>();
        benchmarkSIMD<float, 6, 32>();
        benchmarkSIMD<float, 7, 32>();
        benchmarkSIMD<float, 8, 32>();
        benchmarkSIMD<float, 9, 32>();
        benchmarkSIMD<float, 10, 32>();
        benchmarkSIMD<float, 11, 32>();
        benchmarkSIMD<float, 12, 32>();
        benchmarkSIMD<float, 13, 32>();
        benchmarkSIMD<float, 14, 32>();
        benchmarkSIMD<float, 15, 32>();
        benchmarkSIMD<float, 16, 32>();
    }

} // namespace marvin::testing