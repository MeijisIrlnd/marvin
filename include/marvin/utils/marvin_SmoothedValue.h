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
namespace marvin::utils {
    /**
        \brief Enum to configure SmoothedValue to either use linear smoothing, or exponential (lowpass) smoothing.
    */
    enum class SmoothingType {
        Linear,
        Exponential
    };
    /**
    \brief A utility class to smooth discrete values over a given period.

    Mainly useful to help with zippering caused by discrete parameter updates. Can be configured to either smooth linearly or exponentially to the target value.
    */
    template <FloatType SampleType, SmoothingType Type>
    class SmoothedValue {
    public:
        /**
            Sets the period of the smoothing, and internally sets the current value to the target value.
            \param stepsSamples The period of the smoothing, in samples.
        */
        void reset(int stepsSamples);
        /**
            Sets the period of the smoothing, and internally sets the current value to the target value.
            \param sampleRate The currently configured sample rate.
            \param timeMs The period of the smoothing, in milliseconds.
        */
        void reset(double sampleRate, double timeMs);
        /**
            Sets both the current value, and the target value to interpolate to.
            \param newValue The new target (and current) value.
        */
        void setCurrentAndTargetValue(SampleType newValue);

        /**
            Sets the value the smoother should interpolate to.
            \param newValue The new target value.
        */
        void setTargetValue(SampleType newValue);
        /**
            Performs a single tick of the smoothing function.<br>
            If the SmoothedValue is configured to use SmoothingType::Linear, the formula is
            `v = v+slew`, where slew has been calculated based on the target value.<br>
            If the SmoothedValue is configured to use SmoothingType::Exponential, the formula is
            `v = v + (target = v) * slew`.
            \return The smoothed value.
        */
        [[nodiscard]] SampleType operator()() noexcept;
        /**
            Checks if the smoother has reached its target value.
            \returns Whether the smoother has reached its target value.
        */
        [[nodiscard]] bool isSmoothing() noexcept;

    private:
        int m_duration{ 1 };
        int m_samplesRemaining{ 0 };
        SampleType m_currentValue{ static_cast<SampleType>(0.0) };
        SampleType m_targetValue{ static_cast<SampleType>(0.0) };
        SampleType m_slew{ static_cast<SampleType>(0.0) };
    };
} // namespace marvin::utils
#endif
