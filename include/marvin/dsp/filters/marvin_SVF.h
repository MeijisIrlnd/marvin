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
    template <FloatType SampleType>
    struct SVFResult {
        SampleType highpass;
        SampleType bandpass;
        SampleType lowpass;
        SampleType normalisedBandpass;
        SampleType bandshelf;
        SampleType lowShelf;
        SampleType highShelf;
    };

    template <FloatType SampleType>
    class SVF {
    public:
        void initialise(double sampleRate);
        void setFrequency(SampleType newFrequency);
        void setResonance(SampleType newResonance);
        void setGainDb(SampleType newGainDb);
        [[nodiscard]] SVFResult<SampleType> operator()(SampleType x);

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