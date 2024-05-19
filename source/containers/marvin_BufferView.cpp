
// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/containers/marvin_BufferView.h"
#include "marvin/library/marvin_Concepts.h"
#include <cassert>
namespace marvin::containers {
    template <FloatType SampleType>
    BufferView<SampleType>::BufferView(SampleType* const* samples, size_t nChannels, size_t nSamples) : m_samples(samples),
                                                                                                        m_nChannels(nChannels),
                                                                                                        m_nSamples(nSamples) {
    }

    template <FloatType SampleType>
    size_t BufferView<SampleType>::getNumChannels() const noexcept {
        return m_nChannels;
    }

    template <FloatType SampleType>
    size_t BufferView<SampleType>::getNumSamples() const noexcept {
        return m_nSamples;
    }

    template <FloatType SampleType>
    SampleType* const* BufferView<SampleType>::getArrayOfReadPointers() const noexcept {
        return m_samples;
    }

    template <FloatType SampleType>
    SampleType* const* BufferView<SampleType>::getArrayOfWritePointers() noexcept {
        return m_samples;
    }

    template <FloatType SampleType>
    std::span<SampleType> BufferView<SampleType>::operator[](size_t channel) noexcept {
        assert(channel <= m_nChannels);
        return { m_samples[channel], m_nSamples };
    }

    template <FloatType SampleType>
    std::span<const SampleType> BufferView<SampleType>::operator[](size_t channel) const noexcept {
        assert(channel <= m_nChannels);
        return { m_samples[channel], m_nSamples };
    }

    template struct BufferView<float>;
    template struct BufferView<double>;
} // namespace marvin::containers