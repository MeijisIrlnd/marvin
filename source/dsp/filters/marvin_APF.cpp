// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/dsp/filters/marvin_APF.h"
namespace marvin::dsp::filters {

    template <FloatType SampleType>
    void LatticeAPF<SampleType>::initialise(double sampleRate) {
        m_delay.setMaximumDelayInSamples(static_cast<int>(sampleRate));
        m_delay.initialise(sampleRate);
    }

    template <FloatType SampleType>
    void LatticeAPF<SampleType>::setDelay(SampleType newDelaySamples) noexcept {
        m_delay.setDelay(newDelaySamples);
    }

    template <FloatType SampleType>
    void LatticeAPF<SampleType>::setCoeff(SampleType newCoeff) noexcept {
        m_coeff = newCoeff;
    }

    template <FloatType SampleType>
    SampleType LatticeAPF<SampleType>::operator()(SampleType x) noexcept {
        const auto delayOut = this->m_delay.popSample();
        const auto feedBack = delayOut * (static_cast<SampleType>(-1.0) * m_coeff);
        const auto delayIn = x + feedBack;
        this->m_delay.pushSample(delayIn);
        const auto feedForward = delayIn * this->m_coeff;
        const auto out = delayOut + feedForward;
        return out;
    }

    template <FloatType SampleType>
    SampleType LatticeAPF<SampleType>::tap(SampleType delaySamples) noexcept {
        const auto delayed = this->m_delay.popSample(delaySamples, false);
        return delayed;
    }

    template <FloatType SampleType>
    void LatticeAPF<SampleType>::reset() noexcept {
        m_delay.reset();
    }

    template class LatticeAPF<float>;
    template class LatticeAPF<double>;
} // namespace marvin::dsp::filters
