// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/oscillators/marvin_Oscillator.h"
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
        auto x = (2.0f * phase) - 1.0f;
        return x;
    }

    [[nodiscard]] float naiveTri(float phase) { 
        auto x = -1.0f + (2.0f * phase);
        x = 2.0f * (std::fabs(x) - 0.5f);
        return x;
    }

    [[nodiscard]] float naiveSquare(float phase) { 
        const auto value = phase < 0.5f ? 1.0f : -1.0f;
        return value;
    }

    [[nodiscard]] float naivePulse(float phase,float pulsewidth) { 
        const auto value = phase < pulsewidth ? 1.0f : -1.0f;
        return value;
    }

    void initialiseOsc(oscillators::OscillatorBase<float>* oscillator, float frequency, double sampleRate) { 
        oscillator->initialise(sampleRate);
        oscillator->setFrequency(frequency);
    }

    void initialiseMultiOsc(oscillators::MultiOscillator<float>& osc, float frequency, float pulsewidth, double sampleRate) { 
        osc.initialise(sampleRate);
        osc.setFrequency(frequency);
        osc.setPulsewidth(pulsewidth);
    }

    static auto rd = std::random_device();

    TEST_CASE("Test Oscillators") { 
        using namespace oscillators;
        constexpr auto sampleRate{ 44100.0 };
        constexpr auto frequency{ 1.0f };
        constexpr auto numIterations{ 100 };
        constexpr auto pulsewidth{ 0.25f };

        SineOscillator<float> sine;
        TriOscillator<float, BlepState::Off> tri;
        SawOscillator<float, BlepState::Off> saw;
        SquareOscillator<float, BlepState::Off> square;
        PulseOscillator<float, BlepState::Off> pulse;
        NoiseOscillator<float> noise{ rd };

        MultiOscillator<float, BlepState::Off> multiSine{ rd, MultiOscillator<float>::SHAPE::SINE };
        MultiOscillator<float, BlepState::Off> multiTri{ rd, MultiOscillator<float>::SHAPE::TRIANGLE };
        MultiOscillator<float, BlepState::Off> multiSaw{ rd, MultiOscillator<float>::SHAPE::SAW };
        MultiOscillator<float, BlepState::Off> multiSquare{ rd, MultiOscillator<float>::SHAPE::SQUARE };
        MultiOscillator<float, BlepState::Off> multiPulse{ rd, MultiOscillator<float>::SHAPE::PULSE };

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
        for(auto i = 0; i < numIterations; ++i) { 
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
}
