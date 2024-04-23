// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_OSCILLATOR_H
#define MARVIN_OSCILLATOR_H
#include "marvin/library/marvin_Concepts.h"
#include "marvin/library/marvin_PropagateConst.h"
#include "marvin/math/marvin_LeakyIntegrator.h"
#include "marvin/utils/marvin_Random.h"
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::oscillators {
    namespace BlepState {
        struct Off {};
        struct On {};
    } // namespace BlepState

    template <class T>
    concept BlepType = requires {
        std::is_same_v<T, BlepState::Off> ||
            std::is_same_v<T, BlepState::On>;
    };

    template <FloatType SampleType>
    class OscillatorBase {
    public:
        virtual ~OscillatorBase() noexcept = default;
        virtual void initialise(double sampleRate);
        [[nodiscard]] virtual SampleType operator()() noexcept = 0;
        [[nodiscard]] virtual SampleType operator()(SampleType phase) noexcept = 0;
        virtual void reset() noexcept;

        void setFrequency(SampleType newFrequency) noexcept;

    protected:
        void incrementPhase() noexcept;
        double m_sampleRate{};

        SampleType m_phaseIncrement{ static_cast<SampleType>(0.0) };
        SampleType m_phase{ static_cast<SampleType>(0.0) };
    };

    template <FloatType SampleType>
    class SineOscillator final : public OscillatorBase<SampleType> {
    public:
        ~SineOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;
    };

    template <FloatType SampleType, BlepType Blep = BlepState::Off>
    class TriOscillator final : public OscillatorBase<SampleType> {
    public:
        ~TriOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;

    private:
        math::LeakyIntegrator<SampleType> m_integrator;
    };

    template <FloatType SampleType, BlepType Blep = BlepState::Off>
    class SawOscillator final : public OscillatorBase<SampleType> {
    public:
        ~SawOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;
    };

    template <FloatType SampleType, BlepType Blep = BlepState::Off>
    class SquareOscillator final : public OscillatorBase<SampleType> {
    public:
        ~SquareOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;
    };

    template <FloatType SampleType, BlepType Blep = BlepState::Off>
    class PulseOscillator final : public OscillatorBase<SampleType> {
    public:
        ~PulseOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;
        void setPulsewidth(SampleType newPulsewidth) noexcept;

    private:
        SampleType m_pulsewidth{ 0.5 };
    };

    template <FloatType SampleType>
    class NoiseOscillator final : public OscillatorBase<SampleType> {
    public:
        explicit NoiseOscillator(std::random_device& rd);
        ~NoiseOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;

    private:
        utils::Random m_rng;
    };

    template <FloatType SampleType, BlepType Blep = BlepState::Off>
    class MultiOscillator final {
    public:
        enum class SHAPE {
            SINE,
            TRIANGLE,
            SAW,
            SQUARE,
            PULSE,
            NOISE
        };
        explicit MultiOscillator(std::random_device& rd);
        explicit MultiOscillator(std::random_device& rd, SHAPE shape);

        void initialise(double sampleRate);
        [[nodiscard]] SampleType operator()() noexcept;
        void reset() noexcept;

        void setShape(SHAPE shape);
        void setFrequency(SampleType newFrequency) noexcept;
        void setPulsewidth(SampleType newPulsewidth) noexcept;

    private:
        void incrementPhase() noexcept;
        double m_sampleRate{};
        SHAPE m_shape{ SHAPE::SINE };
        SampleType m_phase{ static_cast<SampleType>(0.0) };
        SampleType m_phaseIncrement{ static_cast<SampleType>(0.0) };
        SineOscillator<SampleType> m_sine;
        TriOscillator<SampleType, Blep> m_tri;
        SawOscillator<SampleType, Blep> m_saw;
        SquareOscillator<SampleType, Blep> m_square;
        PulseOscillator<SampleType, Blep> m_pulse;
        NoiseOscillator<SampleType> m_noise;
    };
} // namespace marvin::oscillators
#include "marvin/library/marvin_DisableWarnings.h"
#endif
