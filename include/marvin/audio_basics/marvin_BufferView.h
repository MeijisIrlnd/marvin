// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_BUFFER_H
#define MARVIN_BUFFER_H
#include "marvin/library/marvin_Concepts.h"
#include <span>
namespace marvin::audiobasics {
    template <FloatType SampleType>
    struct BufferView final {
        BufferView(SampleType** samples, size_t nChannels, size_t nSamples);
        [[nodiscard]] size_t getNumChannels() const noexcept;
        [[nodiscard]] size_t getNumSamples() const noexcept;
        SampleType* const* getArrayOfReadPointers() const noexcept;
        SampleType* const* getArrayOfWritePointers() noexcept;
        [[nodiscard]] std::span<SampleType> operator[](size_t channel) noexcept;
        [[nodiscard]] std::span<const SampleType> operator[](size_t channel) const noexcept;

    private:
        SampleType** m_samples;
        size_t m_nChannels;
        size_t m_nSamples;
    };
} // namespace marvin::audiobasics
#endif
