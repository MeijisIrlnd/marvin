
// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/audio_basics/marvin_BufferView.h"
#include "marvin/library/marvin_Concepts.h"
namespace marvin::audiobasics {
    template <FloatType SampleType>
    BufferView<SampleType>::BufferView(SampleType** samples, int nChannels, int nSamples) : m_samples(samples),
                                                                                            m_nChannels(nChannels),
                                                                                            m_nSamples(nSamples) {
    }

    template <FloatType SampleType>
    int BufferView<SampleType>::getNumChannels() const noexcept {
        return m_nChannels;
    }

    template <FloatType SampleType>
    int BufferView<SampleType>::getNumSamples() const noexcept {
        return m_nSamples;
    }

    template <FloatType SampleType>
    SampleType* const* BufferView<SampleType>::read() const noexcept {
        return m_samples;
    }

    template <FloatType SampleType>
    SampleType* const* BufferView<SampleType>::write() noexcept {
        return m_samples;
    }

    template class BufferView<float>;
    template class BufferView<double>;
} // namespace marvin::audiobasics