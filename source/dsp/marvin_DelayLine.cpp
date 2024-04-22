// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/dsp/marvin_DelayLine.h"
#include <algorithm>
#include <cmath>
#include "marvin/library/enable_warnings.h"
namespace marvin::dsp {
    template <FloatType SampleType>
    [[nodiscard]] bool isPositiveAndNotGreaterThan(SampleType x, SampleType y) noexcept {
        return x >= 0 && x < y;
    }

    template <FloatType SampleType, InterpType InterpolationType>
    DelayLine<SampleType, InterpolationType>::DelayLine() : DelayLine(0) {
    }

    template <FloatType SampleType, InterpType InterpolationType>
    DelayLine<SampleType, InterpolationType>::DelayLine(int maximumDelayInSamples) {
        m_bufferData.reserve(44100);
        m_sampleRate = 44100.0;
        setMaximumDelayInSamples(maximumDelayInSamples);
    }


    template <FloatType SampleType, InterpType InterpolationType>
    void DelayLine<SampleType, InterpolationType>::setDelay(SampleType newDelayInSamples) {
        auto upperLimit = static_cast<SampleType>(getMaximumDelayInSamples());
        m_delay = std::clamp(newDelayInSamples, static_cast<SampleType>(0), upperLimit);
        m_delayInt = static_cast<int>(std::floor(m_delay));
        m_delayFrac = m_delay - static_cast<SampleType>(m_delayInt);
        updateInternalVariables();
    }

    template <FloatType SampleType, InterpType InterpolationType>
    SampleType DelayLine<SampleType, InterpolationType>::getDelay() const noexcept {
        return m_delay;
    }

    template <FloatType SampleType, InterpType InterpolationType>
    void DelayLine<SampleType, InterpolationType>::initialise(double sampleRate) {
        m_sampleRate = sampleRate;
        m_bufferData.resize(m_totalSize);
        reset();
    }

    template <FloatType SampleType, InterpType InterpolationType>
    void DelayLine<SampleType, InterpolationType>::setMaximumDelayInSamples(int maxDelayInSamples) {
        m_totalSize = std::max(4, maxDelayInSamples + 2);
        m_bufferData.resize(m_totalSize);
        reset();
    }

    template <FloatType SampleType, InterpType InterpolationType>
    int DelayLine<SampleType, InterpolationType>::getMaximumDelayInSamples() const noexcept {
        return m_totalSize;
    }

    template <FloatType SampleType, InterpType InterpolationType>
    void DelayLine<SampleType, InterpolationType>::reset() {
        m_readPos = m_writePos = 0;
        std::fill(m_bufferData.begin(), m_bufferData.end(), static_cast<SampleType>(0.0));
    }

    template <FloatType SampleType, InterpType InterpolationType>
    void DelayLine<SampleType, InterpolationType>::pushSample(SampleType sample) {
        const auto writePos{ static_cast<size_t>(m_writePos) };
        m_bufferData.at(writePos) = sample;
        m_writePos = (m_writePos + m_totalSize - 1) % m_totalSize;
    }

    template <FloatType SampleType, InterpType InterpolationType>
    SampleType DelayLine<SampleType, InterpolationType>::popSample(SampleType delayInSamples, bool updateReadPointer) {
        if (delayInSamples >= 0) {
            setDelay(delayInSamples);
        }
        const auto result = interpolateSample();
        if (updateReadPointer) {
            m_readPos = (m_readPos + m_totalSize - 1) % m_totalSize;
        }
        return result;
    }

    template <FloatType SampleType, InterpType InterpolationType>
    SampleType DelayLine<SampleType, InterpolationType>::interpolateSample() {
        if constexpr (std::is_same_v<InterpolationType, DelayLineInterpolationTypes::None>) {
            const auto index = (m_readPos + m_delayInt) % m_totalSize;
            return m_bufferData.at(index);
        } else if constexpr (std::is_same_v<InterpolationType, DelayLineInterpolationTypes::Linear>) {
            auto index0 = m_readPos + m_delayInt;
            auto index1 = index0 + 1;
            if (index1 >= m_totalSize) {
                index0 %= m_totalSize;
                index1 %= m_totalSize;
            }
            const auto value0 = m_bufferData.at(index0);
            const auto value1 = m_bufferData.at(index1);
            const auto interpolated = value0 + m_delayFrac * (value1 - value0);
            return interpolated;
        } else if constexpr (std::is_same_v<InterpolationType, DelayLineInterpolationTypes::Lagrange3rd>) {
            auto index0 = m_readPos + m_delayInt;
            auto index1 = index0 + 1;
            auto index2 = index0 + 2;
            auto index3 = index0 + 3;
            if (index3 >= m_totalSize) {
                index0 %= m_totalSize;
                index1 %= m_totalSize;
                index2 %= m_totalSize;
                index3 %= m_totalSize;
            }
            auto sample0 = m_bufferData.at(index0);
            auto sample1 = m_bufferData.at(index1);
            auto sample2 = m_bufferData.at(index2);
            auto sample3 = m_bufferData.at(index3);

            const auto d0 = m_delayFrac - 1.0f;
            const auto d1 = m_delayFrac - 2.0f;
            const auto d2 = m_delayFrac - 3.0f;

            const auto c0 = -d0 * d1 * d2 / 6.f;
            const auto c1 = d1 * d2 * 0.5f;
            const auto c2 = -d0 * d2 * 0.5f;
            const auto c3 = d0 * d1 / 6.f;
            return sample0 * c0 + m_delayFrac * (sample1 * c1 + sample2 * c2 + sample3 * c3);
        }
    }

    template <FloatType SampleType, InterpType InterpolationType>
    void DelayLine<SampleType, InterpolationType>::updateInternalVariables() {
        if constexpr (std::is_same_v<InterpolationType, DelayLineInterpolationTypes::Lagrange3rd>) {
            if (m_delayFrac < static_cast<SampleType>(2.0) && m_delayInt >= 1) {
                ++m_delayFrac;
                --m_delayInt;
            }
        }
    }


    template class DelayLine<float, DelayLineInterpolationTypes::None>;
    template class DelayLine<double, DelayLineInterpolationTypes::None>;
    template class DelayLine<float, DelayLineInterpolationTypes::Linear>;
    template class DelayLine<double, DelayLineInterpolationTypes::Linear>;
    template class DelayLine<float, DelayLineInterpolationTypes::Lagrange3rd>;
    template class DelayLine<double, DelayLineInterpolationTypes::Lagrange3rd>;
} // namespace marvin::dsp
#include "marvin/library/disable_warnings.h"
