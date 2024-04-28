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

    /**
        \brief Tag-like structs used to configure classes deriving from `marvin::oscillators::OscillatorBase`.
    */
    namespace BlepState {
        /**
            \brief No PolyBLEP (or BLAMP in the case of `marvin::oscillators::TriOscillator`)
        */
        struct Off {};
        /**
            \brief PolyBLEP (or BLAMP in the case of `marvin::oscillators::TriOscillator`)
        */
        struct On {};
    } // namespace BlepState

    /**
        \brief Constrains T to be either `marvin::oscillators::BlepState::Off`, or `marvin::oscillators::BlepState::On`
    */
    template <class T>
    concept BlepType = requires {
        std::is_same_v<T, BlepState::Off> ||
            std::is_same_v<T, BlepState::On>;
    };

    /**
        \brief Base class for all single-shape oscillator types.
    */
    template <FloatType SampleType>
    class OscillatorBase {
    public:
        virtual ~OscillatorBase() noexcept = default;
        /**
            Initialises the oscillator. Make sure this is called before attempting any audio processing, if relying on internal phase updates!
            \param sampleRate The sample rate the oscillator should run at.
        */
        virtual void initialise(double sampleRate);

        /**
            Processes the next sample of the wave, handling phase increments internally. Make sure to call prepareToPlay and setFrequency before calling this function!
            \return The oscillator's output.
        */
        [[nodiscard]] virtual SampleType operator()() noexcept = 0;
        /**
            Processes the next sample of the wave, using the provided phase - this overload will <b>not</b> update the internal phase of the oscillator, so the caller is entirely responsible for incrementing the phase according to frequency, etc. Does *not* require you to call initialise beforehand for this reason.
            \param phase The phase the oscillator should use, between 0 to 1.
            \return The oscillator's output.

        */
        [[nodiscard]] virtual SampleType operator()(SampleType phase) noexcept = 0;
        /**
            Resets the oscillator to its default state.
        */
        virtual void reset() noexcept;

        /**
            Sets the frequency of the oscillator in Hz. If using the overload of `operator()` that takes a value for phase, this function will do nothing.
            The internal `phaseIncrement` variable this function sets is <b>not</b> atomic, so ensure this is either called on the audio thread, or the audio thread is <b>not</b> running when this function is called.
            \param newFrequency The frequency to set, in Hz
        */
        void setFrequency(SampleType newFrequency) noexcept;

    protected:
        /**
            Called by subclasses to increment the internal phase according to the frequency set in `setFrequency`. Unused if using an external value for phase.
        */
        void incrementPhase() noexcept;

        double m_sampleRate{};
        SampleType m_phaseIncrement{ static_cast<SampleType>(0.0) };
        SampleType m_phase{ static_cast<SampleType>(0.0) };
    };

    /**
        \brief A sine oscillator.
    */
    template <FloatType SampleType>
    class SineOscillator final : public OscillatorBase<SampleType> {
    public:
        ~SineOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;
    };

    /**
        \brief A triangle oscillator, with optional BLAMP.
    */
    template <FloatType SampleType, BlepType Blep = BlepState::Off>
    class TriOscillator final : public OscillatorBase<SampleType> {
    public:
        ~TriOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;

    private:
        math::LeakyIntegrator<SampleType> m_integrator;
    };

    /**
        \brief A sawtooth oscillator, with optional BLEP.
    */
    template <FloatType SampleType, BlepType Blep = BlepState::Off>
    class SawOscillator final : public OscillatorBase<SampleType> {
    public:
        ~SawOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;
    };

    /**
        \brief A square oscillator, with optional BLEP.
    */
    template <FloatType SampleType, BlepType Blep = BlepState::Off>
    class SquareOscillator final : public OscillatorBase<SampleType> {
    public:
        ~SquareOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;
    };

    /**
        \brief A pulse oscillator, with optional BLEP, and pulsewidth control.
    */
    template <FloatType SampleType, BlepType Blep = BlepState::Off>
    class PulseOscillator final : public OscillatorBase<SampleType> {
    public:
        ~PulseOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;
        /**
            Sets the oscillator's pulsewidth. Note that the internal `pulsewidth` variable this function sets is <b>not</b> atomic, so ensure this function is either called on the audio thread, or that the audio thread is not running when this function is called.
            \param newPulsewidth The duration of an oscillation, between 0 and 1, that the pulse should be high for.
        */
        void setPulsewidth(SampleType newPulsewidth) noexcept;

    private:
        SampleType m_pulsewidth{ 0.5 };
    };

    /**
        \brief A white noise oscillator.
    */
    template <FloatType SampleType>
    class NoiseOscillator final : public OscillatorBase<SampleType> {
    public:
        /**
            As it's not recommended to keep multiple instances of `std::random_device` around, it's necessary to pass one in by reference to the constructor, as a seed for the internal rng.
            \param rd The seed generator to use.
        */
        explicit NoiseOscillator(std::random_device& rd);
        ~NoiseOscillator() noexcept override = default;
        [[nodiscard]] SampleType operator()() noexcept override;
        /**
            As NoiseOscillator has no concept of phase, this overload is exactly identical to the internal phase overload.
            \param phase Unused in this case.
        */
        [[nodiscard]] SampleType operator()(SampleType phase) noexcept override;

    private:
        utils::Random m_rng;
    };

    /**
        \brief A switchable multi-shape oscillator, with optional BLEP.
    */
    template <FloatType SampleType, BlepType Blep = BlepState::Off>
    class MultiOscillator final {
    public:
        /**
            The available shapes to set the MultiOsc to use.
        */
        enum class SHAPE {
            SINE,
            TRIANGLE,
            SAW,
            SQUARE,
            PULSE,
            NOISE
        };
        /**
            As NoiseOscillator requires a `std::random_device` parameter in its constructor, it's also needed here to forward to the internal instance of NoiseOscillator.
            Uses `SHAPE::SINE` as the default shape.
            \param rd The seed generator for the internal NoiseOscillator to use.
        */
        explicit MultiOscillator(std::random_device& rd);
        /**
            Sets the current shape to the value of the shape parameter.
            \param rd The seed generator for the internal NoiseOscillator to use.
            \param shape The shape the MultiOscillator should be configured to use.
        */
        explicit MultiOscillator(std::random_device& rd, SHAPE shape);

        /**
            Initialises all child oscillators. Make sure to call this before any calls to the call operator.
            \param sampleRate The sample rate the oscillator should use.
        */
        void initialise(double sampleRate);
        /**
            Processes a sample with the configured oscillator. As this class calls the external-phase overloads of the child oscillators and manages the phase itself, all child oscillators will share the same phase when switched.
            \return The selected oscillator's output.
        */
        [[nodiscard]] SampleType operator()() noexcept;
        /**
            Resets all internal oscillators to their initial states.
        */
        void reset() noexcept;

        /**
            Configures the shape the MultiOscillator should output. Note that the internal shape variable is not atomic, and is read directly on the audio thread - so make sure this function is called on the audio thread, or that the audio thread is <b>not</b> running when this function is called.
            \param shape The shape the MultiOscillator should output.
        */
        void setShape(SHAPE shape);
        /**
            Sets the frequency the MultiOscillator should output. This propagates to all internal oscillators. Note that the internal `phaseIncrement` variable this function sets is not atomic, so ensure this function is either called on the audio thread, or that the audio thread is <b>not</b> running when this function is called.
            \param newFrequency The frequency the MultiOscillator should process at.
        */
        void setFrequency(SampleType newFrequency) noexcept;
        /**
            Sets the pulsewidth the MultiOscillator should use when configured with `SHAPE::PULSE`. Note that the internal `pulsewidth` variable the PulseOscillator class uses is <b>not</b> atomic, so ensure this function is either called on the audio thread, or that the audio thread is <b>not</b> running when this function is called.
        */
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
