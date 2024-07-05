// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================
#include <marvin/dsp/filters/marvin_SVF.h>
#include <cassert>
#include <numbers>

namespace marvin::dsp::filters {
    template <FloatType SampleType>
    void SVF<SampleType>::initialise(double sampleRate) {
        m_sampleRate = sampleRate;
    }

    template <FloatType SampleType>
    void SVF<SampleType>::setFrequency(SampleType newFrequency) {
        assert(m_sampleRate != 0);
        // Omega C is angular frequency, and then T is period -
        // Period = 1 / f
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto wd = newFrequency * twoPi;
        const auto T = static_cast<SampleType>(1.0) / static_cast<SampleType>(m_sampleRate);
        const auto wa = (static_cast<SampleType>(2.0) / T) * std::tan(wd * T / static_cast<SampleType>(2.0));
        m_g = (wa * T) / static_cast<SampleType>(2.0);
    }

    template <FloatType SampleType>
    void SVF<SampleType>::setResonance(SampleType newResonance) {
        m_R = static_cast<SampleType>(1.0) - newResonance;
    }

    template <FloatType SampleType>
    void SVF<SampleType>::setGainDb(SampleType newGainDb) {
        const auto exponent = newGainDb / static_cast<SampleType>(20.0);
        const auto lhs = std::pow(10, exponent);
        m_k = lhs - static_cast<SampleType>(1.0);
    }

    template <FloatType SampleType>
    SVFResult<SampleType> SVF<SampleType>::operator()(SampleType x) {
        const auto twoR = static_cast<SampleType>(2.0) * m_R;
        // g1=2R+g
        const auto g1 = twoR + m_g;
        const auto twoRg = twoR * m_g;
        const auto g_2 = m_g * m_g;
        // d=1/(1+2Rg+g^2)
        const auto d = static_cast<SampleType>(1.0) / (static_cast<SampleType>(1.0) + twoRg + g_2);
        // HP := (x-g1*s1-s2)*d
        const auto hp = (x - g1 * m_s1 - m_s2) * d;
        // v1 := g*HP;
        const auto v1 = m_g * hp;
        // BP := v1+s1;
        const auto bp = v1 + m_s1;
        //  s1 := BP+v1;
        m_s1 = bp + v1;
        // v2 := g*BP;
        const auto v2 = m_g * bp;
        // LP := v2+s2;
        const auto lp = v2 + m_s2;
        // s2 := LP+v2;
        m_s2 = lp + v2;
        const auto normalisedBandpass = bp * twoR; // unit gain, 0 phase response at Wc
        const auto bandShelf = x + (m_k * normalisedBandpass);
        const auto lowShelf = x + (m_k * lp);
        const auto highShelf = x + (m_k * hp);
        return {
            .highpass = hp,
            .bandpass = bp,
            .lowpass = lp,
            .normalisedBandpass = normalisedBandpass,
            .bandshelf = bandShelf,
            .lowShelf = lowShelf,
            .highShelf = highShelf
        };
    }

    template <FloatType SampleType>
    void SVF<SampleType>::reset() {
        m_s1 = static_cast<SampleType>(0.0);
        m_s2 = static_cast<SampleType>(0.0);
    }

    template class SVF<float>;
    template class SVF<double>;
} // namespace marvin::dsp::filters
