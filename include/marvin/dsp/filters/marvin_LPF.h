// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_LPF_H
#define MARVIN_LPF_H
#include "marvin/library/marvin_Concepts.h"
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::dsp::filters {
    template <FloatType SampleType>
    class LPF {
    public:
        void setCoeff(SampleType newCoeff) noexcept;
        [[nodiscard]] SampleType operator()(SampleType x) noexcept;
        void reset() noexcept;

    private:
        SampleType m_prev{ static_cast<SampleType>(0.0) };
        SampleType m_coeff{ static_cast<SampleType>(0.0) };
    };
} // namespace marvin::dsp::filters
#include "marvin/library/marvin_DisableWarnings.h"
#endif
