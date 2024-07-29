// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/library/marvin_Concepts.h>
#include <marvin/utils/marvin_Utils.h>
#include <iostream>
#include <array>
namespace marvin::utils {
    template <FloatType SampleType>
    struct AudioFile {
        double sampleRate;
        std::uint8_t bitDepth;
        size_t numChannels;
        size_t lengthSamples;
        std::vector<std::vector<SampleType>> data;
    };

    namespace {
        /** @internal */
        struct FMTChunkDescriptor {
            std::int16_t format;
            std::int16_t nChannels;
            std::int32_t sampleRate;
            std::int32_t bytesPerSecond;
            std::int16_t bytesPerBlock;
            std::int16_t bitsPerSample;
        };

        /** @internal */
        std::optional<std::span<char>> findChunk(std::int32_t sequence, std::span<char> data, std::ptrdiff_t offset) {
            auto readPointer = data.data() + offset;
            while (true) {
                const auto id = *reinterpret_cast<std::int32_t*>(readPointer);
                // This is chunkSize - 8 -> so we need to add 8 when we're actually advancing
                const auto chunkSize = *reinterpret_cast<std::int32_t*>(readPointer + 4) + 8;
                if (id == sequence) {
                    std::span<char> resView{ readPointer, static_cast<size_t>(chunkSize) };
                    return resView;
                }
                if ((readPointer + chunkSize) - data.data() >= data.size()) return {};
                readPointer += chunkSize;
            }
        }

        /** @internal */
        bool verifyRIFFChunk(std::span<char> data) {
            auto* readPointer = data.data();
            // First four bytes should be 0x52, 0x49, 0x46, 0x46 ( R I F F )
            if (!(readPointer[0] == 0x52 && readPointer[1] == 0x49 && readPointer[2] == 0x46 && readPointer[3] == 0x46)) return false;
            // Offset by 4 bytes, size should == data.size() - 8
            readPointer += 4;
            const auto reportedSize = *reinterpret_cast<std::int32_t*>(readPointer);
            if (reportedSize != data.size() - 8) return false;
            readPointer += 4;
            // File Format ID -> 0x57 0x41 0x56 0x45 ( W A V E )
            if (!(readPointer[0] == 0x57 && readPointer[1] == 0x41 && readPointer[2] == 0x56 && readPointer[3] == 0x45)) return false;
            return true;
        }

        /** @internal */
        template <FloatType SampleType>
        FMTChunkDescriptor parseFMTChunk(std::span<char> data, AudioFile<SampleType>& dest) {
            auto readPointer = data.data() + 8;
            const auto format = *reinterpret_cast<std::int16_t*>(readPointer);
            readPointer += 2;
            const auto nChannels = *reinterpret_cast<std::int16_t*>(readPointer);
            readPointer += 2;
            const auto sampleRate = *reinterpret_cast<std::int32_t*>(readPointer);
            readPointer += 4;
            const auto bytesPerSec = *reinterpret_cast<std::int32_t*>(readPointer);
            readPointer += 4;
            const auto bytesPerBlock = *reinterpret_cast<std::int16_t*>(readPointer);
            readPointer += 2;
            const auto bitsPerSample = *reinterpret_cast<std::int16_t*>(readPointer);
            dest.sampleRate = static_cast<double>(sampleRate);
            dest.bitDepth = static_cast<std::uint8_t>(bitsPerSample);
            dest.numChannels = static_cast<size_t>(nChannels);
            return {
                .format = format,
                .nChannels = nChannels,
                .sampleRate = sampleRate,
                .bytesPerSecond = bytesPerSec,
                .bytesPerBlock = bytesPerBlock,
                .bitsPerSample = bitsPerSample
            };
        }
        /** @internal */
        template <class T>
        concept FloatTypeOrFixedWidthSignedInteger = FloatType<T> || FixedWidthSignedInteger<T>;

        /** @internal */
        template <FloatType OutputSampleType, FloatTypeOrFixedWidthSignedInteger InputSampleType, size_t BitDepth>
        void parseDataChunkT(std::span<char> data, const FMTChunkDescriptor& fmtDescriptors, AudioFile<OutputSampleType>& dest) {
            constexpr static auto sampleSize = BitDepth / 8;
            std::array<char, sizeof(InputSampleType)> temp;
            std::fill(temp.begin(), temp.end(), 0);
            const auto scalar = std::pow(2, BitDepth - 1);
            const auto totalSamplesInStream = (data.size() / sampleSize);
            const auto numSamples = totalSamplesInStream / fmtDescriptors.nChannels;
            dest.lengthSamples = static_cast<size_t>(numSamples);
            for (auto& channelVec : dest.data) {
                channelVec.reserve(numSamples);
            }
            const auto stride = fmtDescriptors.nChannels * sampleSize;
            for (std::ptrdiff_t pointerOffset = 0; pointerOffset < data.size(); pointerOffset += stride) {
                auto* channelPointer = data.data() + pointerOffset;
                for (std::ptrdiff_t channel = 0; channel < fmtDescriptors.nChannels; ++channel) {
                    // RIFF is always LITTLE endian..
                    // Read SampleSize Bytes..
                    if constexpr (FixedWidthSignedInteger<InputSampleType>) {
                        std::span<unsigned char> currentWord{ reinterpret_cast<unsigned char*>(channelPointer), sampleSize };
                        InputSampleType inputSample{ 0 };
                        for (auto b = 0; b < sampleSize; ++b) {
                            // temp[b] = currentWord[b];
                            inputSample = inputSample + (currentWord[b] << (8 * b));
                        }
                        // auto inputSample = *reinterpret_cast<InputSampleType*>(temp.data());
                        if (BitDepth == 24) {
                        }
                        const auto normalisedSample = static_cast<OutputSampleType>(inputSample) / static_cast<OutputSampleType>(scalar);
                        dest.data[channel].emplace_back(normalisedSample);
                    } else {
                        const auto currentSample = *reinterpret_cast<InputSampleType*>(channelPointer);
                        dest.data[channel].emplace_back(static_cast<OutputSampleType>(currentSample));
                    }
                    channelPointer += sampleSize;
                }
            }
        }

        /** @internal */
        template <FloatType SampleType>
        void parseDataChunk(std::span<char> data, const FMTChunkDescriptor& fmtDescriptors, AudioFile<SampleType>& dest) {
            dest.data.resize(fmtDescriptors.nChannels);
            std::span<char> audioDataView{ data.data() + 8, data.size() - 8 };
            if (fmtDescriptors.format == 1) {
                switch (fmtDescriptors.bitsPerSample) {
                    case 8: {
                        parseDataChunkT<SampleType, std::int8_t, 8>(audioDataView, fmtDescriptors, dest);
                        return;
                    }
                    case 16: {
                        parseDataChunkT<SampleType, std::int16_t, 16>(audioDataView, fmtDescriptors, dest);
                        return;
                    }
                    case 24: {
                        parseDataChunkT<SampleType, std::int32_t, 24>(audioDataView, fmtDescriptors, dest);
                        return;
                    }
                    case 32: {
                        parseDataChunkT<SampleType, std::int32_t, 32>(audioDataView, fmtDescriptors, dest);
                        return;
                    }
                    case 64: {
                        parseDataChunkT<SampleType, std::int64_t, 64>(audioDataView, fmtDescriptors, dest);
                        return;
                    }
                    default: break;
                }
            } else {
                switch (fmtDescriptors.bitsPerSample) {
                    case 32: {
                        parseDataChunkT<SampleType, float, 32>(audioDataView, fmtDescriptors, dest);
                        return;
                    }
                    case 64: {
                        parseDataChunkT<SampleType, double, 64>(audioDataView, fmtDescriptors, dest);
                        return;
                    }
                    default: break;
                }
            }
        }

    } // namespace

    template <FloatType SampleType>
    bool loadWavFile(const std::string& path, AudioFile<SampleType>& dest) {
        std::vector<char> fileContents;
        if (!readBinaryFile(path, fileContents)) return false;
        if (!verifyRIFFChunk(fileContents)) return false;
        // FMT -> 0x66 0x6D 0x74 0x20
        char rawFmtSequence[4] = { 0x66, 0x6D, 0x74, 0x20 };
        const std::int32_t fmtSequence{ *reinterpret_cast<std::int32_t*>(rawFmtSequence) };
        auto fmtOpt = findChunk(fmtSequence, fileContents, 12);
        if (!fmtOpt) return false;
        auto fmtView = *fmtOpt;
        const auto fmtDescriptors = parseFMTChunk(fmtView, dest);

        char rawDataSequence[4] = { 0x64, 0x61, 0x74, 0x61 };
        const auto dataSequence{ *reinterpret_cast<std::int32_t*>(rawDataSequence) };
        const auto startOffset = (fmtView.data() + fmtView.size()) - fileContents.data();
        auto dataOpt = findChunk(dataSequence, fileContents, startOffset);
        if (!dataOpt) return false;
        parseDataChunk(*dataOpt, fmtDescriptors, dest);

        return true;
    }

    template <FloatType SampleType>
    bool loadAudioFile(const std::string& path, AudioFile<SampleType>& dest) {
        // check the extension..
        auto delimPos = path.find_last_of('.');
        if (delimPos == std::string::npos) return false;
        const auto extension = path.substr(delimPos + 1);
        if (extension == "wav") {
            return loadWavFile(path, dest);
        }
        return false;
    }

} // namespace marvin::utils