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
#include <filesystem>
#include <AudioFile.h>

namespace marvin::testing {
    static std::random_device s_rd;
    template <FloatType SampleType>
    void checkGarbage(double sr, SampleType fc, SampleType res) {
        const auto inRange = [](SampleType x) -> bool {
            return std::abs(x) <= static_cast<SampleType>(1.0);
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
            REQUIRE(!std::isnan(results.bandshelf));
            REQUIRE(!std::isinf(results.bandshelf));
            REQUIRE(inRange(results.bandshelf));
            REQUIRE(!std::isnan(results.lowShelf));
            REQUIRE(!std::isinf(results.lowShelf));
            REQUIRE(inRange(results.lowShelf));
            REQUIRE(!std::isnan(results.highShelf));
            REQUIRE(!std::isinf(results.highShelf));
            REQUIRE(inRange(results.highShelf));
        }
    }

    template <FloatType SampleType>
    void renderNoise(double sr, SampleType fc, SampleType res) {
        marvin::dsp::oscillators::NoiseOscillator<SampleType> osc{ s_rd };
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
        destPath += "/FilteredNoise.wav";
        AudioFile<SampleType> f{};
        f.setSampleRate(static_cast<std::uint32_t>(sr));
        f.setAudioBufferSize(1, static_cast<size_t>(sr));
        for (auto i = 0_sz; i < static_cast<size_t>(sr); ++i) {
            const auto noise = osc();
            const auto filteredNoise = svf(noise);
            f.samples[0][i] = filteredNoise.highShelf;
        }
        f.save(destPath.string());
    }

    TEST_CASE("Test SVF") {
        checkGarbage<float>(44100.0, 1000.0f, 0.5f);
        renderNoise<float>(44100.0, 5000.0f, 0.0f);
    }

} // namespace marvin::testing