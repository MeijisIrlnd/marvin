// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/dsp/filters/biquad/marvin_BiquadCoefficients.h"
#include "marvin/dsp/filters/biquad/marvin_SmoothedBiquadCoefficients.h"
#include "marvin/utils/marvin_SmoothedValue.h"
#include <marvin/dsp/filters/biquad/marvin_Biquad.h>
#include <marvin/dsp/filters/biquad/marvin_RBJCoefficients.h>
#include <marvin/math/marvin_Math.h>
#include <marvin/math/marvin_Conversions.h>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>
#include <iostream>
namespace marvin::testing {
    template <FloatType SampleType>
    std::vector<SampleType> generateImpulse(size_t len) {
        std::vector<SampleType> impulse;
        impulse.resize(len);
        std::fill(impulse.begin(), impulse.end(), static_cast<SampleType>(0.0));
        impulse[0] = static_cast<SampleType>(1.0);
        return impulse;
    }

    template <FloatType SampleType, size_t N>
    void testFilter(marvin::dsp::filters::BiquadCoefficients<SampleType> coeffs) {
        auto impulse = generateImpulse<SampleType>(N);
        dsp::filters::Biquad<SampleType, 1> filter;
        filter.setCoeffs(0, coeffs);
        for (auto sample = 0_sz; sample < N; ++sample) {
            const auto out = filter(impulse[sample]);
            REQUIRE((!std::isnan(out) && (!std::isinf(out))));
            REQUIRE((std::abs(out) < 1.0f));
        }
    }

    template <FloatType SampleType, size_t N, utils::SmoothingType Smoothing>
    void testFilterWithSmoothing(marvin::dsp::filters::BiquadCoefficients<SampleType> start, marvin::dsp::filters::BiquadCoefficients<SampleType> end) {
        const auto smoothingStr = Smoothing == utils::SmoothingType::Linear ? "Linear" : "Exponential";
        SECTION(fmt::format("Smoothing<{}>", smoothingStr)) {
            auto impulse = generateImpulse<SampleType>(N);
            dsp::filters::Biquad<SampleType, 1> filter;
            marvin::dsp::filters::SmoothedBiquadCoefficients<SampleType, Smoothing, 1> smoothedCoeffs;
            smoothedCoeffs.reset(N);
            smoothedCoeffs.setCurrentAndTargetCoeffs(0, start);
            smoothedCoeffs.setTargetCoeffs(0, end);
            filter.setCoeffs(0, smoothedCoeffs.current(0));
            for (auto sample = 0_sz; sample < N; ++sample) {
                const auto out = filter(impulse[sample]);
                smoothedCoeffs.interpolate();
                filter.setCoeffs(0, smoothedCoeffs.current(0));
                REQUIRE(((!std::isnan(out) && (!std::isinf(out)))));
                if (std::abs(out) > static_cast<SampleType>(100.0)) {
                    std::cout << out << "\n";
                }
                REQUIRE((std::abs(out) < static_cast<SampleType>(100.0)));
            }
        }
    }

    TEST_CASE("Test Biquad") {
        // Fixed coeffs from https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3/
        dsp::filters::BiquadCoefficients<float> fixedCoeffs = {
            .a0 = 0.00005024141818873903f,
            .a1 = 0.00010048283637747806f,
            .a2 = 0.00005024141818873903,
            .b0 = 1.0f,
            .b1 = -1.979851353142371f,
            .b2 = 0.9800523188151258f,
        };

        testFilter<float, 1000>(fixedCoeffs);
    }
    TEST_CASE("Test RBJ") {
        SECTION("Test lowpass") {
            std::array<float, 9> cutoffs{ 20.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
            std::array<float, 9> qs{ 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f };
            for (auto& c : cutoffs) {
                for (auto& q : qs) {
                    SECTION(fmt::format("Lowpass: c = {}, q = {}", c, q)) {
                        const auto lpfCoeffs = marvin::dsp::filters::rbj::lowpass(44100.0, c, q);
                        testFilter<float, 256>(lpfCoeffs);
                        const auto newCutoff = std::clamp(2.0f * c, 20.0f, 20000.0f);
                        const auto newQ = std::clamp(q * 2.0f, 0.1f, 0.9f);
                        const auto targetCoeffs = marvin::dsp::filters::rbj::lowpass(44100.0, newCutoff, newQ);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Linear>(lpfCoeffs, targetCoeffs);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Exponential>(lpfCoeffs, targetCoeffs);
                    }
                }
            }
        }

        SECTION("Test highpass") {
            std::array<float, 9> cutoffs{ 20.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
            std::array<float, 9> qs{ 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f };
            for (auto& c : cutoffs) {
                for (auto& q : qs) {
                    SECTION(fmt::format("Highpass: c = {}, q = {}", c, q)) {
                        const auto hpfCoeffs = marvin::dsp::filters::rbj::highpass(44100.0, c, q);
                        testFilter<float, 256>(hpfCoeffs);
                        const auto newCutoff = std::clamp(2.0f * c, 20.0f, 20000.0f);
                        const auto newQ = std::clamp(q * 2.0f, 0.1f, 0.9f);
                        const auto targetCoeffs = marvin::dsp::filters::rbj::highpass(44100.0, newCutoff, newQ);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Linear>(hpfCoeffs, targetCoeffs);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Exponential>(hpfCoeffs, targetCoeffs);
                    }
                }
            }
        }

        SECTION("Test peak bandpass") {
            std::array<float, 9> cutoffs{ 20.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
            std::array<float, 9> bandwidths{ 0.1f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f };
            std::array<float, 9> gains{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f };
            for (const auto& c : cutoffs) {
                for (const auto& b : bandwidths) {
                    for (const auto& g : gains) {
                        const auto bpfCoeffs = marvin::dsp::filters::rbj::bandpass(44100, c, b, g);
                        testFilter<float, 256>(bpfCoeffs);
                        const auto newCutoff = std::clamp(2.0f * c, 20.0f, 20000.0f);
                        const auto newBw = std::clamp(b * 2.0f, 0.1f, 0.9f);
                        const auto newGain = std::clamp(g * 2.0f, 0.0f, 1.0f);
                        const auto targetCoeffs = marvin::dsp::filters::rbj::bandpass(44100.0, newCutoff, newBw, newGain);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Linear>(bpfCoeffs, targetCoeffs);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Exponential>(bpfCoeffs, targetCoeffs);
                    }
                }
            }
        }

        SECTION("Test bandpass") {
            std::array<float, 9> cutoffs{ 20.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
            std::array<float, 9> bandwidths{ 0.1f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f };
            for (const auto& c : cutoffs) {
                for (const auto& b : bandwidths) {
                    const auto bpfCoeffs = marvin::dsp::filters::rbj::bandpass(44100.0, c, b);
                    testFilter<float, 256>(bpfCoeffs);
                    const auto newCutoff = std::clamp(2.0f * c, 20.0f, 20000.0f);
                    const auto newBw = std::clamp(b * 2.0f, 0.1f, 0.9f);
                    const auto targetCoeffs = marvin::dsp::filters::rbj::bandpass(44100.0, newCutoff, newBw);
                    testFilterWithSmoothing<float, 256, utils::SmoothingType::Linear>(bpfCoeffs, targetCoeffs);
                    testFilterWithSmoothing<float, 256, utils::SmoothingType::Exponential>(bpfCoeffs, targetCoeffs);
                }
            }
        }

        SECTION("Test notch") {
            std::array<float, 9> cutoffs{ 20.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
            std::array<float, 9> bandwidths{ 0.1f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f };
            for (const auto& c : cutoffs) {
                for (const auto& b : bandwidths) {
                    const auto notchCoeffs = marvin::dsp::filters::rbj::notch(44100.0, c, b);
                    testFilter<float, 256>(notchCoeffs);
                    const auto newCutoff = std::clamp(2.0f * c, 20.0f, 20000.0f);
                    const auto newBw = std::clamp(b * 2.0f, 0.1f, 0.9f);
                    const auto targetCoeffs = marvin::dsp::filters::rbj::notch(44100.0, newCutoff, newBw);
                    testFilterWithSmoothing<float, 256, utils::SmoothingType::Linear>(notchCoeffs, targetCoeffs);
                    testFilterWithSmoothing<float, 256, utils::SmoothingType::Exponential>(notchCoeffs, targetCoeffs);
                }
            }
        }

        SECTION("Test allpass") {
            std::array<float, 9> cutoffs{ 20.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
            std::array<float, 9> qs{ 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f };
            for (const auto& c : cutoffs) {
                for (const auto& q : qs) {
                    const auto apfCoeffs = marvin::dsp::filters::rbj::allpass(44100.0, c, q);
                    testFilter<float, 256>(apfCoeffs);
                    const auto newCutoff = std::clamp(2.0f * c, 20.0f, 20000.0f);
                    const auto newQ = std::clamp(q * 2.0f, 0.1f, 0.9f);
                    const auto targetCoeffs = marvin::dsp::filters::rbj::allpass(44100.0, newCutoff, newQ);
                    testFilterWithSmoothing<float, 256, utils::SmoothingType::Linear>(apfCoeffs, targetCoeffs);
                    testFilterWithSmoothing<float, 256, utils::SmoothingType::Exponential>(apfCoeffs, targetCoeffs);
                }
            }
        }

        SECTION("Test peak") {
            std::array<float, 9> cutoffs{ 20.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
            std::array<float, 9> bandwidths{ 0.1f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f };
            std::array<float, 9> gains{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f };
            for (const auto& c : cutoffs) {
                for (const auto& b : bandwidths) {
                    for (const auto& g : gains) {
                        const auto gainDb = marvin::math::gainToDb(g);
                        const auto peakCoeffs = marvin::dsp::filters::rbj::peak(44100.0, c, b, gainDb);
                        testFilter<float, 256>(peakCoeffs);
                        const auto newCutoff = std::clamp(2.0f * c, 20.0f, 20000.0f);
                        const auto newBw = std::clamp(b * 2.0f, 0.1f, 0.9f);
                        const auto newGain = std::clamp(g * 2.0f, 0.0f, 1.0f);
                        const auto targetCoeffs = marvin::dsp::filters::rbj::peak(44100.0, newCutoff, newBw, newGain);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Linear>(peakCoeffs, targetCoeffs);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Exponential>(peakCoeffs, targetCoeffs);
                    }
                }
            }
        }
        SECTION("Test lowshelf") {
            std::array<float, 9> cutoffs{ 20.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
            std::array<float, 9> slopes{ 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f };
            std::array<float, 9> gains{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f };
            for (const auto& c : cutoffs) {
                for (const auto& s : slopes) {
                    for (const auto& g : gains) {
                        const auto gaindb = marvin::math::gainToDb(g);
                        const auto coeffs = marvin::dsp::filters::rbj::lowShelf(44100.0, c, s, gaindb);
                        testFilter<float, 256>(coeffs);
                        const auto newCutoff = std::clamp(2.0f * c, 20.0f, 20000.0f);
                        const auto newSlope = std::clamp(s * 2.0f, 0.1f, 0.9f);
                        const auto newGain = std::clamp(g * 2.0f, 0.0f, 1.0f);
                        const auto targetCoeffs = marvin::dsp::filters::rbj::lowShelf(44100.0, newCutoff, newSlope, newGain);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Linear>(coeffs, targetCoeffs);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Exponential>(coeffs, targetCoeffs);
                    }
                }
            }
        }
        SECTION("Test highshelf") {
            std::array<float, 9> cutoffs{ 20.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
            std::array<float, 9> slopes{ 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f };
            std::array<float, 9> gains{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f };
            for (const auto& c : cutoffs) {
                for (const auto& s : slopes) {
                    for (const auto& g : gains) {
                        const auto gaindb = marvin::math::gainToDb(g);
                        const auto coeffs = marvin::dsp::filters::rbj::highShelf(44100.0, c, s, gaindb);
                        testFilter<float, 256>(coeffs);
                        const auto newCutoff = std::clamp(2.0f * c, 20.0f, 20000.0f);
                        const auto newSlope = std::clamp(s * 2.0f, 0.1f, 0.9f);
                        const auto newGain = std::clamp(g * 2.0f, 0.0f, 1.0f);
                        const auto targetCoeffs = marvin::dsp::filters::rbj::highShelf(44100.0, newCutoff, newSlope, newGain);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Linear>(coeffs, targetCoeffs);
                        testFilterWithSmoothing<float, 256, utils::SmoothingType::Exponential>(coeffs, targetCoeffs);
                    }
                }
            }
        }
    }

} // namespace marvin::testing
