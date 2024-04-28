// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/dsp/filters/marvin_LPF.h"
#include <cmath>
#include <cassert>

namespace marvin::dsp::filters {

    template <FloatType SampleType>
    void LPF<SampleType>::initialise(double sampleRate) noexcept {
        m_sampleRate = sampleRate;
    }

    template <FloatType SampleType>
    void LPF<SampleType>::setCutoff(SampleType newCutoff) noexcept {
        assert(m_sampleRate != 0.0);
        const auto fs{ static_cast<SampleType>(m_sampleRate) };
        const auto omega{ newCutoff / fs };
        const auto y = static_cast<SampleType>(1.0) - std::cos(omega);
        const auto alpha = (static_cast<SampleType>(-1.0) * y) + std::sqrt(std::pow(y, static_cast<SampleType>(2.0)) + (static_cast<SampleType>(2.0) * y));
        setCoeff(alpha);
    }

    template <FloatType SampleType>
    void LPF<SampleType>::setCoeff(SampleType newCoeff) noexcept {
        m_coeff = newCoeff;
    }


    template <FloatType SampleType>
    [[nodiscard]] SampleType LPF<SampleType>::operator()(SampleType x) noexcept {
        const auto gained = x * m_coeff;
        const auto feedback = m_prev * (static_cast<SampleType>(1.0) - m_coeff);
        const auto forward = gained + feedback;
        m_prev = forward;
        return forward;
    }

    template <FloatType SampleType>
    void LPF<SampleType>::reset() noexcept {
        m_prev = static_cast<SampleType>(0.0);
    }

    template class LPF<float>;
    template class LPF<double>;
} // namespace marvin::dsp::filters
