#include "marvin/utils/marvin_LinearSmoothedValue.h"
#include "marvin/library/enable_warnings.h"
namespace Audio {
    template <FloatType SampleType>
    void LinearSmoothedValue<SampleType>::reset(int steps) {
        m_duration = m_samplesRemaining = steps;
        setCurrentAndTargetValue(m_targetValue);
    }

    template <FloatType SampleType>
    void LinearSmoothedValue<SampleType>::reset(double sampleRate, double timeMs) {
        const auto inSamples = static_cast<int>((timeMs / 1000.0) * sampleRate);
        reset(inSamples);
    }

    template <FloatType SampleType>
    void LinearSmoothedValue<SampleType>::setCurrentAndTargetValue(SampleType newValue) {
        m_currentValue = newValue;
        setTargetValue(newValue);
    }

    template <FloatType SampleType>
    void LinearSmoothedValue<SampleType>::setTargetValue(SampleType newValue) {
        m_slew = (newValue - m_currentValue) / static_cast<SampleType>(m_duration);
        m_targetValue = newValue;
        m_samplesRemaining = m_duration;
    }

    template <FloatType SampleType>
    SampleType LinearSmoothedValue<SampleType>::operator()() noexcept {
        if (!isSmoothing()) return m_targetValue;
        m_currentValue = m_currentValue + m_slew;
        --m_samplesRemaining;
        return m_currentValue;
    }

    template <FloatType SampleType>
    bool LinearSmoothedValue<SampleType>::isSmoothing() noexcept {
        return m_samplesRemaining > 0;
    }

    template class LinearSmoothedValue<float>;
    template class LinearSmoothedValue<double>;
} // namespace Audio
#include "marvin/library/disable_warnings.h"
