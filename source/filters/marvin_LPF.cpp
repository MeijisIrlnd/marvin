// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/filters/marvin_LPF.h"
#include "marvin/library/enable_warnings.h"
namespace Audio {
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
} // namespace Audio
#include "marvin/library/disable_warnings.h"
