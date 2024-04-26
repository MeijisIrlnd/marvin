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
        BufferView(SampleType** samples, int nChannels, int nSamples);
        [[nodiscard]] int getNumChannels() const noexcept;
        [[nodiscard]] int getNumSamples() const noexcept;
        [[nodiscard]] SampleType* const* read() const noexcept;
        [[nodiscard]] SampleType* const* write() noexcept;

    private:
        SampleType** m_samples;
        int m_nChannels;
        int m_nSamples;
    };
} // namespace marvin::audiobasics
#endif
