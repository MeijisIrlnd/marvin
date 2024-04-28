// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/dsp/filters/biquad/marvin_SmoothedBiquadCoefficients.h"
namespace marvin::dsp::filters {
    template <FloatType SampleType>
    void SmoothedBiquadCoefficients<SampleType>::setCurrentAndTargetCoeffs(const BiquadCoefficients<SampleType>& target) noexcept {
        m_current = target;
        setTargetCoeffs(target);
    }

    template <FloatType SampleType>
    void SmoothedBiquadCoefficients<SampleType>::setTargetCoeffs(const BiquadCoefficients<SampleType>& target) noexcept {
        m_target = target;
    }

    template <FloatType SampleType>
    const BiquadCoefficients<SampleType>& SmoothedBiquadCoefficients<SampleType>::current() const noexcept {
        return m_current;
    }

    template <FloatType SampleType>
    void SmoothedBiquadCoefficients<SampleType>::interpolate() noexcept {
        // TODO: make period variable (ask geraint?)
        constexpr auto slew{ static_cast<SampleType>(0.004) };
        m_current.b0 = m_current.b0 - slew * (m_current.b0 - m_target.b0);
        m_current.b1 = m_current.b1 - slew * (m_current.b1 - m_target.b1);
        m_current.b2 = m_current.b2 - slew * (m_current.b2 - m_target.b2);
        m_current.a0 = m_current.a0 - slew * (m_current.a0 - m_target.a0);
        m_current.a1 = m_current.a1 - slew * (m_current.a1 - m_target.a1);
        m_current.a2 = m_current.a2 - slew * (m_current.a2 - m_target.a2);
    }

    template class SmoothedBiquadCoefficients<float>;
    template class SmoothedBiquadCoefficients<double>;
} // namespace marvin::dsp::filters