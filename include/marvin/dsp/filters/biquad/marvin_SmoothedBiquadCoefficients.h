// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_SMOOTHEDBIQUAD_COEFFICIENTS_H
#define MARVIN_SMOOTHEDBIQUAD_COEFFICIENTS_H

#include "marvin/library/marvin_Concepts.h"
#include "marvin/dsp/filters/biquad/marvin_BiquadCoefficients.h"
namespace marvin::dsp::filters {
    template <FloatType SampleType>
    class SmoothedBiquadCoefficients {
    public:
        void setCurrentAndTargetCoeffs(const BiquadCoefficients<SampleType>& target) noexcept;
        void setTargetCoeffs(const BiquadCoefficients<SampleType>& target) noexcept;
        [[nodiscard]] const BiquadCoefficients<SampleType>& current() const noexcept;
        void interpolate() noexcept;

    private:
        BiquadCoefficients<SampleType> m_current{}, m_target{};
    };
} // namespace marvin::dsp::filters
#endif
