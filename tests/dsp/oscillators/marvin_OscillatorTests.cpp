// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/dsp/oscillators/marvin_Oscillator.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <numbers>
#include <random>
namespace marvin::testing {
    [[nodiscard]] float getPhaseIncrement(float frequency, double sampleRate) noexcept {
        const auto period = 1.0f / frequency;
        const auto periodSamples = period * static_cast<float>(sampleRate);
        return 1.0f / periodSamples;
    }

    [[nodiscard]] float naiveSine(float phase) {
        static constexpr auto twoPi = std::numbers::pi_v<float> * 2.0f;
        const auto radianPhase = phase * twoPi;
        const auto sineOut = std::sin(radianPhase);
        return sineOut;
    }

    [[nodiscard]] float naiveSaw(float phase) {
        // Shift this by 0.5
        phase = std::fmod(phase + 0.5f, 1.0f);
        return 2.0f * (phase - std::floor(phase + 0.5f));
    }

    [[nodiscard]] float naiveTri(float phase) {
        return 4.0f * std::abs(phase - std::floor(phase + 0.75f) + 0.25f) - 1.0f;
    }

    [[nodiscard]] float naiveSquare(float phase) {
        return phase < 0.5f ? 1.0f : -1.0f;
    }

    [[nodiscard]] float naivePulse(float phase, float pulsewidth) {
        return phase < pulsewidth ? 1.0f : -1.0f;
    }


    void initialiseOsc(dsp::oscillators::OscillatorBase<float>* oscillator, float frequency, double sampleRate) {
        oscillator->initialise(sampleRate);
        oscillator->setFrequency(frequency);
    }

    void initialiseMultiOsc(dsp::oscillators::MultiOscillator<float>& osc, float frequency, float pulsewidth, double sampleRate) {
        osc.initialise(sampleRate);
        osc.setFrequency(frequency);
        osc.setPulsewidth(pulsewidth);
    }

    static auto rd = std::random_device();

    TEST_CASE("Test oscillators") {
        using namespace dsp::oscillators;
        constexpr auto sampleRate{ 44100.0 };
        constexpr auto frequency{ 1.0f };
        constexpr auto numIterations{ 100 };
        constexpr auto pulsewidth{ 0.25f };

        SECTION("Test oscillator waves") {
            SineOscillator<float> sine;
            TriOscillator<float, Bandlimiting::Off> tri;
            SawOscillator<float, Bandlimiting::Off> saw;
            SquareOscillator<float, Bandlimiting::Off> square;
            PulseOscillator<float, Bandlimiting::Off> pulse;
            NoiseOscillator<float> noise{ rd };

            MultiOscillator<float, Bandlimiting::Off> multiSine{ rd, MultiOscillator<float>::Shape::Sine };
            MultiOscillator<float, Bandlimiting::Off> multiTri{ rd, MultiOscillator<float>::Shape::Triangle };
            MultiOscillator<float, Bandlimiting::Off> multiSaw{ rd, MultiOscillator<float>::Shape::Saw };
            MultiOscillator<float, Bandlimiting::Off> multiSquare{ rd, MultiOscillator<float>::Shape::Square };
            MultiOscillator<float, Bandlimiting::Off> multiPulse{ rd, MultiOscillator<float>::Shape::Pulse };
            MultiOscillator<float, Bandlimiting::Off> multiNoise{ rd, MultiOscillator<float>::Shape::Noise };

            initialiseOsc(&sine, frequency, sampleRate);
            initialiseOsc(&tri, frequency, sampleRate);
            initialiseOsc(&saw, frequency, sampleRate);
            initialiseOsc(&square, frequency, sampleRate);
            initialiseOsc(&pulse, frequency, sampleRate);
            pulse.setPulsewidth(pulsewidth);

            initialiseMultiOsc(multiSine, frequency, pulsewidth, sampleRate);
            initialiseMultiOsc(multiTri, frequency, pulsewidth, sampleRate);
            initialiseMultiOsc(multiSaw, frequency, pulsewidth, sampleRate);
            initialiseMultiOsc(multiSquare, frequency, pulsewidth, sampleRate);
            initialiseMultiOsc(multiPulse, frequency, pulsewidth, sampleRate);

            auto phase{ 0.0f };
            const auto phaseIncrement = getPhaseIncrement(frequency, sampleRate);
            for (auto i = 0; i < numIterations; ++i) {
                const auto sineInternalOut = sine();
                const auto sineExternalOut = sine(phase);
                const auto multiSineOut = multiSine();
                const auto naiveSineOut = naiveSine(phase);
                REQUIRE_THAT(sineInternalOut, Catch::Matchers::WithinRel(naiveSineOut));
                REQUIRE_THAT(sineExternalOut, Catch::Matchers::WithinRel(naiveSineOut));
                REQUIRE_THAT(multiSineOut, Catch::Matchers::WithinRel(sineInternalOut));
                const auto triInternalOut = tri();
                const auto triExternalOut = tri(phase);
                const auto multiTriOut = multiTri();
                const auto naiveTriOut = naiveTri(phase);
                REQUIRE_THAT(triInternalOut, Catch::Matchers::WithinRel(naiveTriOut));
                REQUIRE_THAT(triExternalOut, Catch::Matchers::WithinRel(naiveTriOut));
                REQUIRE_THAT(multiTriOut, Catch::Matchers::WithinRel(triInternalOut));
                const auto sawInternalOut = saw();
                const auto sawExternalOut = saw(phase);
                const auto multiSawOut = multiSaw();
                const auto naiveSawOut = naiveSaw(phase);
                REQUIRE_THAT(sawInternalOut, Catch::Matchers::WithinRel(naiveSawOut));
                REQUIRE_THAT(sawExternalOut, Catch::Matchers::WithinRel(naiveSawOut));
                REQUIRE_THAT(multiSawOut, Catch::Matchers::WithinRel(sawInternalOut));
                const auto squareInternalOut = square();
                const auto squareExternalOut = square(phase);
                const auto multiSquareOut = multiSquare();
                const auto naiveSquareOut = naiveSquare(phase);
                REQUIRE_THAT(squareInternalOut, Catch::Matchers::WithinRel(naiveSquareOut));
                REQUIRE_THAT(squareExternalOut, Catch::Matchers::WithinRel(naiveSquareOut));
                REQUIRE_THAT(multiSquareOut, Catch::Matchers::WithinRel(squareInternalOut));
                const auto pulseInternalOut = pulse();
                const auto pulseExternalOut = pulse(phase);
                const auto multiPulseOut = multiPulse();
                const auto naivePulseOut = naivePulse(phase, pulsewidth);
                REQUIRE_THAT(pulseInternalOut, Catch::Matchers::WithinRel(naivePulseOut));
                REQUIRE_THAT(pulseExternalOut, Catch::Matchers::WithinRel(naivePulseOut));
                REQUIRE_THAT(multiPulseOut, Catch::Matchers::WithinRel(pulseInternalOut));
                const auto noiseOut = noise();
                REQUIRE((noiseOut >= -1.0f && noiseOut <= 1.0f));

                phase += phaseIncrement;
            }
        }
        SECTION("Test PolyBLEP Ranges") {
            // I don't know enough about BLEP to test the actual functionality
            TriOscillator<float, Bandlimiting::On> triOsc;
            SawOscillator<float, Bandlimiting::On> sawOsc;
            SquareOscillator<float, Bandlimiting::On> squareOsc;
            PulseOscillator<float, Bandlimiting::On> pulseOsc;
            initialiseOsc(&triOsc, frequency, sampleRate);
            initialiseOsc(&sawOsc, frequency, sampleRate);
            initialiseOsc(&squareOsc, frequency, sampleRate);
            initialiseOsc(&pulseOsc, frequency, sampleRate);
            pulseOsc.setPulsewidth(pulsewidth);
            for (auto i = 0; i < numIterations; ++i) {
                const auto triOut = triOsc();
                REQUIRE(!std::isnan(triOut));
                REQUIRE(!std::isinf(triOut));
                REQUIRE(std::abs(triOut) <= 1.0f);
                const auto sawOut = sawOsc();
                REQUIRE(!std::isnan(sawOut));
                REQUIRE(!std::isinf(sawOut));
                REQUIRE(std::abs(sawOut) <= 1.0f);
                const auto squareOut = squareOsc();
                REQUIRE(!std::isnan(squareOut));
                REQUIRE(!std::isinf(squareOut));
                REQUIRE(std::abs(squareOut) <= 1.0f);
                const auto pulseOut = pulseOsc();
                REQUIRE(!std::isnan(pulseOut));
                REQUIRE(!std::isinf(pulseOut));
                REQUIRE(std::abs(pulseOut) <= 1.0f);
            }
        }
    }
} // namespace marvin::testing
