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
namespace marvin::dsp {
    /**
        \brief Enum to configure the type of interpolation an instance of marvin::dsp::DelayLine should use.
    */
    enum class DelayLineInterpolationType {
        None,
        Linear,
        Lagrange3rd
    };

    /**
        \brief A fractional delay line implementation, with configurable interpolation types.

        For available options for interpolation, see the marvin::dsp::DelayLineInterpolationType enum class.
    */
    template <FloatType SampleType, DelayLineInterpolationType InterpolationType = DelayLineInterpolationType::Linear>
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
        /**
            Returns the currently set delay time.
            \return The delay time, in samples.
        */
        [[nodiscard]] SampleType getDelay() const noexcept;
        /**
            Initialises the delay line. Make sure to call this function before any audio calls!
            \param sampleRate The sample rate the DelayLine should run at.
        */
        void initialise(double sampleRate);
        /**
            Sets the maximum length of the internal buffer. <b>Will</b> allocate if more space is required by the internal vector, so it's best to call this from the parent's `initialise()` function before any processing,
            with the max length the delay line will ever be.
            \param maxDelayInSamples The amount of samples to allocate in the buffer.
        */
        void setMaximumDelayInSamples(int maxDelayInSamples);
        /**
            Returns the maximum length of the internal buffer.
            \return The maximum delay in samples.
        */
        [[nodiscard]] int getMaximumDelayInSamples() const noexcept;
        /**
            Clears the internal buffer, and resets the DelayLine to its initialised state.
        */
        void reset();
        /**
            Pushes a sample into the DelayLine.
            \param sample The sample to add to the DelayLine.
        */
        void pushSample(SampleType sample);
        /**
            Returns the sample at the internal readPos dictated by the earlier calls to `setDelay` if `delayInSamples` is -1,
            or the sample at `writePos + delayInSamples` if it isn't. If updateReadPointer is false, the internal read pointer does <b>not</b> update,
            which is useful for a multitap configuration.
            \param delayInSamples An optional offset from writePos to read from.
            \param updateReadPointer Whether or not the internal read pointer should advance its position.
            \return The sample at the internal readPos if `delayInSamples == -1`, the sample at `writePos + delayInSamples` otherwise.
        */
        [[nodiscard]] SampleType popSample(SampleType delayInSamples = -1, bool updateReadPointer = true);

        /**
         * Retrieves the current position of the read head. Note that the read head runs backwards (ie towards zero).
         * \return The current position of the read head.
         */
        [[nodiscard]] int getReadPos() const noexcept;

        /**
         * Retrives the current position of the write head. Note that the write head runs backwards (ie towards zero).
         * \return The current position of the write head.
         */
        [[nodiscard]] int getWritePos() const noexcept;

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
#endif
