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
#include <numbers>
#include <cassert>

namespace marvin::utils {
    template <FloatType SampleType, SmoothingType Type>
    void SmoothedValue<SampleType, Type>::reset(int steps, bool skipRemaining) {
        m_duration = steps;
        if (skipRemaining) return;
        m_samplesRemaining = steps;
        setCurrentAndTargetValue(m_targetValue);
    }

    template <FloatType SampleType, SmoothingType Type>
    void SmoothedValue<SampleType, Type>::reset(double sampleRate, double timeMs, bool skipRemaining) {
        const auto inSamples = static_cast<int>((timeMs / 1000.0) * sampleRate);
        reset(inSamples, skipRemaining);
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
            m_samplesRemaining = m_duration;
        } else {
            // If your slew factor is slew, the distance to the target is (1 - slew)^n after the nth iteration.
            // If you want that distance to be 1%, then you want (1 - slew)^n = 0.01.
            // Take the log of both sides: n*log(1 - slew) = log(0.01)
            // So your n = log(0.01)/log(1 - slew), and then round up to get a whole number of iterations.
            // constexpr auto pc = static_cast<SampleType>(0.5);
            // const auto t = m_duration / -std::log(pc / static_cast<SampleType>(100.0));

            // m_slew = static_cast<SampleType>(1.0) / (static_cast<SampleType>(m_duration) + static_cast<SampleType>(1.0));
            const auto exponent = static_cast<SampleType>(-1.0) / static_cast<SampleType>(m_duration);
            m_slew = static_cast<SampleType>(1.0) - std::pow(std::numbers::e_v<SampleType>, exponent);
            const auto nItersTo1Pc = std::log(static_cast<SampleType>(0.01)) / std::log(static_cast<SampleType>(1.0) - m_slew);
            m_samplesRemaining = static_cast<int>(std::ceil(nItersTo1Pc));
        }
        m_targetValue = newValue;
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
    bool SmoothedValue<SampleType, Type>::isSmoothing() const noexcept {
        return m_samplesRemaining > 0;
    }

    template <FloatType SampleType, SmoothingType Type>
    int SmoothedValue<SampleType, Type>::getRemainingSamples() const noexcept {
        return m_samplesRemaining;
    }

    template <FloatType SampleType, SmoothingType Type>
    SampleType SmoothedValue<SampleType, Type>::getTargetValue() const noexcept {
        return m_targetValue;
    }

    template class SmoothedValue<float, SmoothingType::Linear>;
    template class SmoothedValue<float, SmoothingType::Exponential>;
    template class SmoothedValue<double, SmoothingType::Linear>;
    template class SmoothedValue<double, SmoothingType::Exponential>;
} // namespace marvin::utils
