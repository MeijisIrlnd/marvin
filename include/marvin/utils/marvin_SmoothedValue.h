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
            Sets the period of the smoothing, and optionally sets the current value to the target value.
            If Type == SmoothingType::Linear, the interpolation will take exactly this many samples. If Type == SmoothingType::Exponential,
            then the period actually specifies the time (in samples) it will take for the smoother to reach 63.2% (approx `1 - 1/e`) of the target value.
            This is the mathematical definition of an exponential smoother's time constant, so was chosen for that reason. If you'd instead like to set the time taken to reach
            some arbitary percent distance from the target value, then you can calculate the value for it with something like the below snippet.
            ```cpp
            constexpr static auto withinPc{ 10.0f }; // Within 10% of target value
            const auto period = stepsSamples / -std::log(withinPc / 100.0f); // period samples to reach 90% of the target value.
            ```

            \param stepsSamples The period of the smoothing, in samples.
            \param skipRemaining If true, sets the current value to the target value.
        */
        void reset(int stepsSamples, bool skipRemaining = true);
        /**
            Sets the period of the smoothing, and optionally sets the current value to the target value.
            \param sampleRate The currently configured sample rate.
            \param timeMs The period of the smoothing, in milliseconds.
            \param skipRemaining If true, sets the current value to the target value.
        */
        void reset(double sampleRate, double timeMs, bool skipRemaining = true);
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
        [[nodiscard]] bool isSmoothing() const noexcept;

        /**
            \return The number of samples left to reach the target (if linear), or the number of samples left to reach 1% of the target (if exponential)
        */
        [[nodiscard]] int getRemainingSamples() const noexcept;

        /**
            \return The value the smoother is smoothing towards.
        */
        [[nodiscard]] SampleType getTargetValue() const noexcept;

    private:
        int m_duration{ 1 };
        int m_samplesRemaining{ 0 };
        SampleType m_currentValue{ static_cast<SampleType>(0.0) };
        SampleType m_targetValue{ static_cast<SampleType>(0.0) };
        SampleType m_slew{ static_cast<SampleType>(0.0) };
        SampleType m_approxTarget{};
    };
} // namespace marvin::utils
#endif
