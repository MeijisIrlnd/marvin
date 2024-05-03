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
#include "marvin/utils/marvin_SmoothedValue.h"
#include <array>
#include <cassert>
namespace marvin::dsp::filters {

    /**
        \brief Helper class to simplify smoothly changing BiquadCoefficients with no zippering.

        See marvin::utils::SmoothingType for options for smoothing. Internally just runs a marvin::utils::SmoothedValue per biquad coefficient.
        <br>Usage example:
        ```cpp
            using namespace marvin::dsp::filters;
            using namespace marvin::utils;
            class FilterSmoothingDemo final {
            public:
                void initialise(double sampleRate) {
                    constexpr static auto periodMs{ 5.0f }; // 5ms duration
                    m_sampleRate = sampleRate;
                    m_updateRate = static_cast<int>((periodMs / 1000.0f) * static_cast<float>(sampleRate));
                    const auto lpfCoeffs = rbj::lowpass<float>(sampleRate, 2000.0f, 0.5f);
                    const auto hpfCoeffs = rbj::highpass<float>(sampleRate, 200.0f, 0.5f);
                    m_smoothedCoeffs.reset(sampleRate, periodMs);
                    m_smoothedCoeffs.setCurrentAndTargetCoeffs(lpfCoeffs, 0); // stage 0
                    m_smoothedCoeffs.setCurrentAndTargetCoeffs(hpfCoeffs, 1); // stage 1
                    m_filter.setCoeffs(0, m_smoothedCoeffs.current(0));
                    m_filter.setCoeffs(1, m_smoothedCoeffs.current(1));
                }

                [[nodiscard]] float operator()(float x, float newLpfCutoff, float newHpfCutoff) noexcept {
                    if(m_samplesUntilUpdate == 0) {
                        const auto newLpfCoeffs = rbj::lowpass<float>(m_sampleRate, newLpfCutoff, 0.5f);
                        const auto newHpfCoeffs = rbj::highpass<float>(m_sampleRate, newHpfCutoff, 0.5f);
                        m_smoothedCoeffs.setTargetCoeffs(newLpfCoeffs, 0);
                        m_smoothedCoeffs.setTargetCoeffs(newHpfCoeffs, 1);
                        m_samplesUntilUpdate = m_updateRate;
                    }
                    m_smoothedCoeffs(); // Interpolate the coeffs
                    m_filter.setCoeffs(0, m_smoothedCoeffs.current(0));
                    m_filter.setCoeffs(1, m_smoothedCoeffs.current(1));
                    const auto filtered = m_filter(x);
                    --m_samplesUntilUpdate;
                    return filtered;
                }

                void reset() noexcept {
                    m_filter.reset();
                }

            private:
                double m_sampleRate;
                int m_updateRate;
                int m_samplesUntilUpdate{ 0 };
                Biquad<float, 2> m_filter;
                SmoothedBiquadCoefficients<float, SmoothingType::Exponential, 2> m_smoothedCoeffs{};
            };
        ```
    */
    template <FloatType SampleType, utils::SmoothingType InterpolationType, size_t NumStages>
    requires(NumStages > 0)
    class SmoothedBiquadCoefficients final {
    public:
        /**
            Sets the duration of the smoothing, and sets the target value to the current value.
            \param periodSamples The duration of the smoothing, in samples.
        */
        void reset(int periodSamples) noexcept {
            for (auto& stage : m_stages) {
                stage.reset(periodSamples);
            }
        }

        /**
            Sets the duration of the smoothing, and sets the target value to the current value.
            \param sampleRate The currently used sample rate.
            \param timeMs The duration of the smoothing, in milliseconds.
        */
        void reset(double sampleRate, double timeMs) noexcept {
            for (auto& stage : m_stages) {
                stage.reset(sampleRate, timeMs);
            }
        }

        /**
            Sets both the current value of the coeffs, and the target value of the coeffs for a given `stage`.
            \param stage The stage index to target.
            \param target The coeffs to set.
        */
        void setCurrentAndTargetCoeffs(size_t stage, BiquadCoefficients<SampleType> target) noexcept {
            assert(stage < NumStages);
            m_stages[stage].setCurrentAndTargetCoeffs(target);
        }

        /**
            Sets the target value for the coeffs to smooth to for a give `stage`.
            \param stage The stage index to target.
            \param target The coeffs to set.
        */
        void setTargetCoeffs(size_t stage, BiquadCoefficients<SampleType> target) noexcept {
            assert(stage < NumStages);
            m_stages[stage].setTargetCoeffs(target);
        }

        /**
            Retrieves the smoothed coeffs for a given `stage`.
            \param stage The index of the stage to retrieve.
            \return The smoothed coefficients for the target `stage`.
        */
        [[nodiscard]] BiquadCoefficients<SampleType> current(size_t stage) const noexcept {
            assert(stage < NumStages);
            return m_stages[stage].current();
        }

        /**
            Performs a single tick of smoothing towards the target in all stages.
        */
        void operator()() noexcept {
            for (auto& stage : m_stages) {
                stage();
            }
        }

    private:
        class CoeffSmoothingStage final {
        public:
            void reset(int periodSamples) noexcept {
                for (auto& s : m_smoothers) {
                    s.reset(periodSamples);
                }
            }

            void reset(double sampleRate, double timeMs) noexcept {
                for (auto& s : m_smoothers) {
                    s.reset(sampleRate, timeMs);
                }
            }

            void setCurrentAndTargetCoeffs(BiquadCoefficients<SampleType> newCoeffs) noexcept {
                m_smoothers[0].setCurrentAndTargetValue(newCoeffs.a0);
                m_smoothers[1].setCurrentAndTargetValue(newCoeffs.a1);
                m_smoothers[2].setCurrentAndTargetValue(newCoeffs.a1);
                m_smoothers[3].setCurrentAndTargetValue(newCoeffs.b0);
                m_smoothers[4].setCurrentAndTargetValue(newCoeffs.b1);
                m_smoothers[5].setCurrentAndTargetValue(newCoeffs.b2);
            }

            void setTargetCoeffs(BiquadCoefficients<SampleType> newCoeffs) noexcept {
                m_smoothers[0].setTargetValue(newCoeffs.a0);
                m_smoothers[1].setTargetValue(newCoeffs.a1);
                m_smoothers[2].setTargetValue(newCoeffs.a1);
                m_smoothers[3].setTargetValue(newCoeffs.b0);
                m_smoothers[4].setTargetValue(newCoeffs.b1);
                m_smoothers[5].setTargetValue(newCoeffs.b2);
            }

            void operator()() noexcept {
                m_current = {
                    .a0 = m_smoothers[0](),
                    .a1 = m_smoothers[1](),
                    .a2 = m_smoothers[2](),
                    .b0 = m_smoothers[3](),
                    .b1 = m_smoothers[4](),
                    .b2 = m_smoothers[5]()
                };
            }

            [[nodiscard]] BiquadCoefficients<SampleType> current() const noexcept {
                return m_current;
            }

        private:
            std::array<marvin::utils::SmoothedValue<SampleType, InterpolationType>, 6> m_smoothers;
            BiquadCoefficients<SampleType> m_current{};
        };

        std::array<CoeffSmoothingStage, NumStages> m_stages;
    };
} // namespace marvin::dsp::filters
#endif
