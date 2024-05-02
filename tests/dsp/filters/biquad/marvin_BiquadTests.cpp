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
#include <marvin/dsp/filters/biquad/marvin_Biquad.h>
#include <marvin/dsp/filters/biquad/marvin_RBJCoefficients.h>
#include <marvin/math/marvin_Math.h>
#include <catch2/catch_test_macros.hpp>
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
                    const auto lpfCoeffs = marvin::dsp::filters::rbj::lowpass(44100.0, c, q);
                    testFilter<float, 256>(lpfCoeffs);
                }
            }
        }

        SECTION("Test highpass") {
            std::array<float, 9> cutoffs{ 20.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
            std::array<float, 9> qs{ 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f };
            for (auto& c : cutoffs) {
                for (auto& q : qs) {
                    const auto hpfCoeffs = marvin::dsp::filters::rbj::highpass(44100.0, c, q);
                    testFilter<float, 256>(hpfCoeffs);
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
                    }
                }
            }
        }
    }

} // namespace marvin::testing
