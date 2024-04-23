// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_LINEARSMOOTHEDVALUE_H
#define MARVIN_LINEARSMOOTHEDVALUE_H
#include "marvin/library/marvin_Concepts.h"
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::utils {
    template <FloatType SampleType>
    class LinearSmoothedValue {
    public:
        void reset(int stepsSamples);
        void reset(double sampleRate, double timeMs);
        void setCurrentAndTargetValue(SampleType newValue);
        void setTargetValue(SampleType newValue);
        [[nodiscard]] SampleType operator()() noexcept;
        [[nodiscard]] bool isSmoothing() noexcept;

    private:
        int m_duration{ 1 };
        int m_samplesRemaining{ 0 };
        SampleType m_currentValue{ static_cast<SampleType>(0.0) };
        SampleType m_targetValue{ static_cast<SampleType>(0.0) };
        SampleType m_slew{ static_cast<SampleType>(0.0) };
    };
} // namespace marvin::utils
#include "marvin/library/marvin_DisableWarnings.h"
#endif
