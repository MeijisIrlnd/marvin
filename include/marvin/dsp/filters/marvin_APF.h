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
    template <FloatType SampleType>
    class LatticeAPF final {
    public:
        ~LatticeAPF() noexcept = default;
        void initialise(double sampleRate);
        void setCoeff(SampleType newCoeff) noexcept;
        void setDelay(SampleType newDelaySamples) noexcept;
        [[nodiscard]] SampleType operator()(SampleType x) noexcept;
        void reset() noexcept;
        [[nodiscard]] SampleType tap(SampleType delaySamples) noexcept;

    private:
        DelayLine<SampleType, DelayLineInterpolationTypes::Linear> m_delay;
        SampleType m_coeff{ 0.0 };
    };

} // namespace marvin::dsp::filters
#include "marvin/library/marvin_DisableWarnings.h"
#endif
