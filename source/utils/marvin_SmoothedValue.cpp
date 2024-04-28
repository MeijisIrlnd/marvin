// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/utils/marvin_SmoothedValue.h"
#include <cmath>

namespace marvin::utils {
    template <FloatType SampleType, SmoothingType Type>
    void SmoothedValue<SampleType, Type>::reset(int steps) {
        m_duration = m_samplesRemaining = steps;
        setCurrentAndTargetValue(m_targetValue);
    }

    template <FloatType SampleType, SmoothingType Type>
    void SmoothedValue<SampleType, Type>::reset(double sampleRate, double timeMs) {
        const auto inSamples = static_cast<int>((timeMs / 1000.0) * sampleRate);
        reset(inSamples);
    }

    template <FloatType SampleType, SmoothingType Type>
    void SmoothedValue<SampleType, Type>::setCurrentAndTargetValue(SampleType newValue) {
        m_currentValue = newValue;
        setTargetValue(newValue);
    }

    template <FloatType SampleType, SmoothingType Type>
    void SmoothedValue<SampleType, Type>::setTargetValue(SampleType newValue) {
        if constexpr (Type == SmoothingType::Linear) {
            m_slew = (newValue - m_currentValue) / static_cast<SampleType>(m_duration);
        } else {
            m_slew = static_cast<SampleType>(1.0) / static_cast<SampleType>(m_duration) + static_cast<SampleType>(1.0);
        }
        m_targetValue = newValue;
        m_samplesRemaining = m_duration;
    }

    template <FloatType SampleType, SmoothingType Type>
    SampleType SmoothedValue<SampleType, Type>::operator()() noexcept {
        if (!isSmoothing()) return m_targetValue;
        if constexpr (Type == SmoothingType::Linear) {
            m_currentValue = m_currentValue + m_slew;
        } else {
            m_currentValue = m_currentValue + (m_targetValue - m_currentValue) * m_slew;
        }
        --m_samplesRemaining;
        return m_currentValue;
    }

    template <FloatType SampleType, SmoothingType Type>
    bool SmoothedValue<SampleType, Type>::isSmoothing() noexcept {
        if constexpr (Type == SmoothingType::Linear) {
            return m_samplesRemaining > 0;
        } else {
            return std::abs(m_targetValue - m_currentValue) <= static_cast<SampleType>(1e-6);
        }
    }

    template class SmoothedValue<float, SmoothingType::Linear>;
    template class SmoothedValue<float, SmoothingType::Exponential>;
    template class SmoothedValue<double, SmoothingType::Linear>;
    template class SmoothedValue<double, SmoothingType::Exponential>;
} // namespace marvin::utils
