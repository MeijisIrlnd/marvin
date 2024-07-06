// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/dsp/filters/marvin_SVF.h>
#include <marvin/library/marvin_Literals.h>
#include <marvin/dsp/oscillators/marvin_Oscillator.h>
#include <marvin/utils/marvin_Utils.h>
#include <catch2/catch_test_macros.hpp>
#include <AudioFile.h>
#include <filesystem>
#include <array>
#include <iostream>


namespace marvin::testing {
    static std::random_device s_rd;
    template <FloatType SampleType>
    void checkGarbage(double sr, SampleType fc, SampleType res) {
        const auto inRange = [](SampleType x) -> bool {
            const auto res = std::abs(x) <= static_cast<SampleType>(1.1);
            if (!res) {
                std::cout << x << "\n";
            }
            return res;
        };

        std::vector<SampleType> impulse(static_cast<size_t>(sr), static_cast<SampleType>(0.0));
        impulse.front() = static_cast<SampleType>(1.0);
        marvin::dsp::filters::SVF<SampleType> svf;
        svf.initialise(sr);
        svf.setFrequency(fc);
        svf.setResonance(res);
        for (auto sample = 0_sz; sample < impulse.size(); ++sample) {
            const auto results = svf(impulse[sample]);
            REQUIRE(!std::isnan(results.highpass));
            REQUIRE(!std::isinf(results.highpass));
            REQUIRE(inRange(results.highpass));
            REQUIRE(!std::isnan(results.bandpass));
            REQUIRE(!std::isinf(results.bandpass));
            REQUIRE(inRange(results.bandpass));
            REQUIRE(!std::isnan(results.lowpass));
            REQUIRE(!std::isinf(results.lowpass));
            REQUIRE(inRange(results.lowpass));
            REQUIRE(!std::isnan(results.normalisedBandpass));
            REQUIRE(!std::isinf(results.normalisedBandpass));
            REQUIRE(inRange(results.normalisedBandpass));
            REQUIRE(!std::isnan(results.bandShelf));
            REQUIRE(!std::isinf(results.bandShelf));
            REQUIRE(inRange(results.bandShelf));
            REQUIRE(!std::isnan(results.lowShelf));
            REQUIRE(!std::isinf(results.lowShelf));
            REQUIRE(inRange(results.lowShelf));
            REQUIRE(!std::isnan(results.highShelf));
            REQUIRE(!std::isinf(results.highShelf));
            REQUIRE(inRange(results.highShelf));
            REQUIRE(!std::isnan(results.notch));
            REQUIRE(!std::isinf(results.notch));
            REQUIRE(inRange(results.notch));
            REQUIRE(!std::isnan(results.allpass));
            REQUIRE(!std::isinf(results.allpass));
            REQUIRE(inRange(results.allpass));
        }
    }
#if defined(MARVIN_ANALYSIS)
    template <FloatType SampleType, marvin::dsp::filters::SVF<SampleType>::FilterType Type, marvin::dsp::oscillators::MultiOscillator<SampleType>::Shape OscType>
    void renderFilter(const std::string& name, double sr, SampleType fc, SampleType res) {
        marvin::dsp::oscillators::MultiOscillator<SampleType> osc{ s_rd };
        osc.initialise(sr);
        osc.setShape(OscType);
        osc.setFrequency(static_cast<SampleType>(120.0));
        marvin::dsp::filters::SVF<SampleType> svf;
        svf.initialise(sr);
        svf.setFrequency(fc);
        svf.setResonance(res);
        svf.setGainDb(static_cast<SampleType>(-12));
        const auto exePathOpt = utils::getCurrentExecutablePath();
        REQUIRE(exePathOpt);
        const auto& exePath = *exePathOpt;
        std::filesystem::path asFsPath{ exePath };
        std::filesystem::path parent = asFsPath.parent_path();
        auto destPath = parent;
        destPath += "/" + name;
        AudioFile<SampleType> f{};
        f.setSampleRate(static_cast<std::uint32_t>(sr));
        f.setAudioBufferSize(1, static_cast<size_t>(sr));
        for (auto i = 0_sz; i < static_cast<size_t>(sr); ++i) {
            const auto noise = osc();
            const auto filteredNoise = svf(Type, noise);
            f.samples[0][i] = filteredNoise;
        }
        f.save(destPath.string());
    }
#endif

    TEST_CASE("Test SVF") {
        constexpr static auto sr{ 100.0 };
        std::array<float, 4> cutoffs{ 20.0f, 100.0f, 500.0f, 1000.0f };
        std::array<float, 4> resonances{ 0.0f, 0.1f, 0.25f, 0.5f };
        for (auto& c : cutoffs) {
            for (auto& r : resonances) {
                checkGarbage<float>(sr, c, r);
                checkGarbage<double>(sr, static_cast<double>(c), static_cast<double>(r));
            }
        }
#if defined(MARVIN_ANALYSIS)
        using FilterType = marvin::dsp::filters::SVF<float>::FilterType;
        using OscType = marvin::dsp::oscillators::MultiOscillator<float>::Shape;
        renderFilter<float, FilterType::Highpass, OscType::Saw>("SVFHighpassFilter.wav", 44100.0, 1000.0f, 0.5f);
        renderFilter<float, FilterType::Bandpass, OscType::Saw>("SVFBandpassFilter.wav", 44100.0, 1000.0f, 0.5f);
        renderFilter<float, FilterType::Lowpass, OscType::Saw>("SVFLowpassFilter.wav", 44100.0, 1000.0f, 0.5f);
        renderFilter<float, FilterType::NormalisedBandpass, OscType::Saw>("SVFNormalisedBandpassFilter.wav", 44100.0, 1000.0f, 0.5f);
        renderFilter<float, FilterType::BandShelf, OscType::Saw>("SVFBandShelfFilter.wav", 44100.0, 1000.0f, 0.5f);
        renderFilter<float, FilterType::LowShelf, OscType::Saw>("SVFLowShelfFilter.wav", 44100.0, 1000.0f, 0.5f);
        renderFilter<float, FilterType::HighShelf, OscType::Saw>("SVFHighShelfFilter.wav", 44100.0, 1000.0f, 0.5f);
        renderFilter<float, FilterType::Notch, OscType::Saw>("SVFNotchFilter.wav", 44100.0, 1000.0f, 0.5f);
        renderFilter<float, FilterType::Allpass, OscType::Saw>("SVFAllpassFilter.wav", 44100.0, 1000.0f, 0.5f);
#endif
    }

} // namespace marvin::testing