// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_DELAYLINE_H
#define MARVIN_DELAYLINE_H
#include "marvin/library/marvin_Concepts.h"
#include <vector>
#include <type_traits>
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::dsp {
    /**
        \brief Tag-like structs used to initialise `marvin::dsp::DelayLine`.
    */
    namespace DelayLineInterpolationTypes {
        /**
            \brief No interpolation.
        */
        struct None {};
        /**
            \brief Linear interpolation.
        */
        struct Linear {};
        /**
            \brief Lagrange 3rd-order interpolation
        */
        struct Lagrange3rd {};
    } // namespace DelayLineInterpolationTypes

    /**
        \brief Constrains T to be one of the structs specified in the `marvin::dsp::DelayLneInterpolationTypes` namespace.
    */
    template <class T>
    concept InterpType = requires {
        std::is_same_v<T, DelayLineInterpolationTypes::None> ||
            std::is_same_v<T, DelayLineInterpolationTypes::Linear> ||
            std::is_same_v<T, DelayLineInterpolationTypes::Lagrange3rd>;
    };

    /**
        \brief A fractional delay line implementation, with configurable interpolation types.
    */
    template <FloatType SampleType, InterpType InterpolationType = DelayLineInterpolationTypes::Linear>
    class DelayLine {
    public:
        /**
            Constructs a DelayLine without initialising the `maximumDelayInSamples` - make sure to call `setMaximumDelayInSamples()` before trying to use it!
        */
        DelayLine();
        /**
            Constructs a DelayLine, and initialises `maximumDelayInSamples`.
            \param maximumDelayInSamples The max size of the internal buffer.
        */
        explicit DelayLine(int maximumDelayInSamples);
        /**
            Sets the delay time (in samples) to `newDelayInSamples`. Clamps to be in the range 0 to `maximumDelayInSamples`.
            Note that the internal `delay` variable this sets is <b>not</b> atomic, so ensure that this function is either called on the audio-thread, or called when the audio-thread is <b>not</b> running.
            \param newDelayInSamples The new delay to use, in samples.
        */
        void setDelay(SampleType newDelayInSamples);
        [[nodiscard]] SampleType getDelay() const noexcept;
        void initialise(double sampleRate);
        void setMaximumDelayInSamples(int maxDelayInSamples);
        [[nodiscard]] int getMaximumDelayInSamples() const noexcept;
        void reset();
        void pushSample(SampleType sample);
        [[nodiscard]] SampleType popSample(SampleType delayInSamples = -1, bool updateReadPointer = true);

    private:
        [[nodiscard]] SampleType interpolateSample();
        void updateInternalVariables();
        double m_sampleRate{};
        std::vector<SampleType> m_bufferData{};
        int m_writePos{ 0 }, m_readPos{ 0 };
        SampleType m_delay{ 0.0 };
        SampleType m_delayFrac{ 0.0 };
        int m_delayInt{ 0 };
        int m_totalSize{ 4 };
    };

} // namespace marvin::dsp
#include "marvin/library/marvin_DisableWarnings.h"
#endif
