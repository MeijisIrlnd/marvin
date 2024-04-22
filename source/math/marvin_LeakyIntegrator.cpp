// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/math/marvin_LeakyIntegrator.h"
#include "marvin/library/enable_warnings.h"
namespace marvin::math {
    template <FloatType SampleType>
    SampleType LeakyIntegrator<SampleType>::operator()(SampleType x, SampleType a) noexcept {
        // y[n] = A * x[n] + (1 - A) * y[n-1]
        const auto res = a * x + (1 - a) * m_prev;
        m_prev = res;
        return res;
    }

    template class LeakyIntegrator<float>;
    template class LeakyIntegrator<double>;
} // namespace marvin::math
#include "marvin/library/disable_warnings.h"
