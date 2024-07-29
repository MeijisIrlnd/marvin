// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/utils/marvin_FormatReader.h>
#include <marvin/utils/marvin_Utils.h>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <AudioFile.h>

namespace marvin::testing {
    template <FloatType SampleType>
    void testReader(const std::string& path, const std::string& destPath) {
        utils::AudioFile<SampleType> res;
        REQUIRE(utils::loadAudioFile(path, res));
        AudioFile<SampleType> destFile;
        destFile.setSampleRate(res.sampleRate);
        destFile.setBitDepth(res.bitDepth);
        destFile.setNumChannels(res.numChannels);
        destFile.setNumSamplesPerChannel(res.lengthSamples);
        for (auto channel = 0; channel < res.numChannels; ++channel) {
            for (auto sample = 0; sample < res.lengthSamples; ++sample) {
                destFile.samples[channel][sample] = res.data[channel][sample];
            }
        }
        destFile.save(destPath);
    }

    TEST_CASE("Test FormatReader") {
        const auto exePathOpt = utils::getCurrentExecutablePath();
        REQUIRE(exePathOpt);
        const auto& exePath = *exePathOpt;
        std::filesystem::path asFsPath{ exePath };
        std::filesystem::path parent = asFsPath.parent_path();
        auto sourceWav = parent;
        sourceWav += "/break.wav";
        auto destWav = parent;
        destWav += "/RoundTrip.wav";
        // testReader<float>(sourceWav.string());
        testReader<float>("C:/Users/Syl/Downloads/Sinead.wav", destWav.string());
    }
} // namespace marvin::testing
