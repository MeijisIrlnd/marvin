// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_APF_H
#define MARVIN_APF_H
#include "marvin/dsp/marvin_DelayLine.h"
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::dsp::filters {
    /**
        \brief A two multiply first order Schroeder allpass filter.
    */
    template <FloatType SampleType>
    class LatticeAPF final {
    public:
        ~LatticeAPF() noexcept = default;
        /**
            Initialises the APF. Ensure this function is called before any calls to the call operator.
            \param sampleRate The sample rate the filter should use.
        */
        void initialise(double sampleRate);
        /**
            Sets the feedback/feedforward coefficient to use. At values >= 1, the filter becomes unstable, so be careful!.<br>
            Note that the internal `coeff` variable this function sets is <b>not</b> atomic, so ensure that this function is either called on the audio thread, or that the audio thread is <b>not</b> running when you call it.
            \param newCoeff The feedback/feedforward coefficient to use.
        */
        void setCoeff(SampleType newCoeff) noexcept;
        /**
            Sets the delay to use. Note that the delay line has a somewhat-arbitrary max of `sampleRate` samples, so be sure not to set the delay to anything above that.
            \param newDelaySamples The delay to use, in samples.
        */
        void setDelay(SampleType newDelaySamples) noexcept;
        /**
            Processes the input through the APF, and returns the filtered sample.
            \param x The sample to filter.
            \return The filtered sample.
        */
        [[nodiscard]] SampleType operator()(SampleType x) noexcept;
        /**
            Resets the filter to it's initial state.
        */
        void reset() noexcept;
        /**
            Retrieve a given sample-into the delay line, without updating the read head. Niche-ly useful in the case of something like a Dattorro reverb, for example.
            \param delaySamples The offset from the current read head to read from.
            \return The sample at the desired offset.
        */
        [[nodiscard]] SampleType tap(SampleType delaySamples) noexcept;

    private:
        DelayLine<SampleType, DelayLineInterpolationTypes::Linear> m_delay;
        SampleType m_coeff{ 0.0 };
    };

} // namespace marvin::dsp::filters
#include "marvin/library/marvin_DisableWarnings.h"
#endif
