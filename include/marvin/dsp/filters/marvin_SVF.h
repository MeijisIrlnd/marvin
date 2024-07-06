// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_SVF_H
#define MARVIN_SVF_H

#include <marvin/library/marvin_Concepts.h>
namespace marvin::dsp::filters {
    /**
        \brief POD Struct containing the results from a tick on an instance of an `SVF`.

        highpass, bandpass and lowpass are fairly self explanatory, except the bandpass does <b>not</b> have unity gain. A unity gain and zero phase
        version can be accessed from `normalisedBandpass`. The low and high shelves are naive implementations with a slight boost or cut just above the cutoff.
     */
    template <FloatType SampleType>
    struct SVFResult {
        SampleType highpass;
        SampleType bandpass;
        SampleType lowpass;
        SampleType normalisedBandpass;
        SampleType bandShelf;
        SampleType lowShelf;
        SampleType highShelf;
        SampleType notch;
        SampleType allpass;
    };

    /**
        \brief A TPT State Variable Filter, based on the structure from [Vadim Zavalishin's The Art of VA Filter Design](https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.1.0.pdf#chapter.4)
     */
    template <FloatType SampleType>
    class SVF {
    public:
        /**
            Represents the available filter types the SVF can process.
         */
        enum class FilterType {
            Highpass,
            Bandpass,
            Lowpass,
            NormalisedBandpass,
            BandShelf,
            LowShelf,
            HighShelf,
            Notch,
            Allpass
        };

        /**
            Initialises the filter. This <b>must</b> be called before calling `setFrequency()` or `operator()`.
            \param sampleRate The sample rate the filter should process at.
         */
        void initialise(double sampleRate);

        /**
            Sets the cutoff frequency of the filter.
            \param newFrequency The new filter cutoff in Hz.
         */
        void setFrequency(SampleType newFrequency);

        /**
            Sets the resonance of the filter directly - a value of 1 achieves self oscillation, a value of 0 is no resonance.
            \param newResonance The new filter resonance, between 0 and 1.
         */
        void setResonance(SampleType newResonance);

        /**
            Sets the gain in dB for the bandshelf, lowshelf and highshelf taps. Ignored in the other filter paths.
            \param newGainDb The new gain in dB.
         */
        void setGainDb(SampleType newGainDb);

        /**
            Processes a sample through the filter, and returns the input filtered through each filter type (with an SVF this is relatively cheap, and allows for custom switching algorithms on the user side).
            \param x The sample to process.
            \return An `SVFResult<SampleType>` containing the filtered results for each filter type.
         */
        [[nodiscard]] SVFResult<SampleType> operator()(SampleType x);

        /**
            Processes a sample through the filter, with a single given filter type.
            \param type The FilterType to use.
            \param x The sample to process.
            \return The filtered sample.
         */
        [[nodiscard]] SampleType operator()(FilterType type, SampleType x);

        /**
            Resets the filter to its initial state.
         */
        void reset();

    private:
        double m_sampleRate;
        SampleType m_g;
        SampleType m_R;
        SampleType m_k{ static_cast<SampleType>(0.0) };
        SampleType m_s1{ static_cast<SampleType>(0.0) };
        SampleType m_s2{ static_cast<SampleType>(0.0) };
    };
} // namespace marvin::dsp::filters

#endif