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
    /**
    \brief A utility class to smooth discrete values over a given period.

    Mainly useful to help with zippering caused by discrete parameter updates.
    */
    template <FloatType SampleType>
    class LinearSmoothedValue {
    public:
        /**
            Sets the internal variables to their targets, and sets the period of the smoothing.
            \param stepsSamples The duration of the smoothing, in samples.
        */
        void reset(int stepsSamples);
        /**
            Sets the internal variables to their targets, and sets the period of the smoothing.
            \param sampleRate The sample rate the LinearSmoothedValue should operate at.
            \param timeMs The duration of the smoothing, in milliseconds.
        */
        void reset(double sampleRate, double timeMs);
        /**
            Sets both the current value, and the target value of the smoother. Use this function to initialise the value of the LinearSmoothedValue.
            \param newValue The new current and target value.
        */
        void setCurrentAndTargetValue(SampleType newValue);
        /**
            Sets the target value of the smoother - the LinearSmoothedValue will then smooth between the internal `currentValue`, and this target value.
            \param newValue The new target value.
        */
        void setTargetValue(SampleType newValue);
        /**
            Performs a single tick of the smoothing from the current value to the target value.
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
#include "marvin/library/marvin_DisableWarnings.h"
#endif
