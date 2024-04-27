// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_BIQUAD_H
#define MARVIN_BIQUAD_H
#include "marvin/library/marvin_Concepts.h"
#include "marvin/library/marvin_Literals.h"
#include "marvin/dsp/filters/biquad/marvin_BiquadCoefficients.h"
#include <array>
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::dsp::filters {
    /**
        \brief A cascading direct form ii biquad filter.
    */
    template <FloatType SampleType, size_t NumStages>
    class Biquad final {
    public:
        /**
            Sets the coeffs the Biquad should use. Note that BiquadCoefficients is trivially copyable (and is just a POD type), so this function can be (and should be) called on the audio thread - the BiquadCoefficients class' members aren't atomic.
            \param stage The stage to assign these coefficients to. <b>Must</b> be less than `NumStages`.
            \param coeffs The coeffs to set.
        */
        void setCoeffs(size_t stage, BiquadCoefficients<SampleType> coeffs) noexcept {
            m_coeffs[stage] = coeffs;
        }

        /**
            Processes a sample through the biquad cascade.
            \param x The sample to filter.
            \return The filtered sample.
        */
        [[nodiscard]] SampleType operator()(SampleType x) noexcept {
            for (auto stage = 0_sz; stage < NumStages; ++stage) {
                const auto [b0, b1, b2, a0, a1, a2] = m_coeffs[stage];
                auto& delay = m_delays[stage];
                const auto y = static_cast<SampleType>(1.0) / b0 * (a0 * x + a1 * delay.x_z1 + a2 * delay.x_z2 + b1 + delay.y_z1 + b2 * delay.y_z2);
                delay.update(x, y);
                x = y;
            }
        }

        /**
            Initialises the filter to its default state (does <b>not</b> zero the coefficients).
        */
        void reset() noexcept {
            for (auto& d : m_delays) {
                d.reset();
            }
        }

    private:
        struct BiquadDelay final {
            SampleType x_z1, x_z2;
            SampleType y_z1, y_z2;

            void operator()(SampleType x, SampleType y) noexcept {
                x_z2 = x_z1;
                x_z1 = x;
                y_z2 = y_z1;
                y_z1 = y;
            }

            void reset() noexcept {
                x_z1 = x_z2 = y_z1 = y_z2 = static_cast<SampleType>(0.0);
            }
        };
        std::array<BiquadDelay, NumStages> m_delays;
        std::array<BiquadCoefficients<SampleType>, NumStages> m_coeffs;
    };

} // namespace marvin::dsp::filters
#include "marvin/library/marvin_DisableWarnings.h"
#endif
