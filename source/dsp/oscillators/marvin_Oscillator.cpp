// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/dsp/oscillators/marvin_Oscillator.h"
#include <numbers>
#include <cmath>
#include <random>

namespace marvin::dsp::oscillators {
    template <FloatType SampleType>
    SampleType polyBlep(SampleType t, SampleType phaseIncrement) noexcept {
        const auto dt = phaseIncrement;
        if (t < dt) {
            t /= dt;
            return t + t - t * t - static_cast<SampleType>(1.0);
        } else if (t > static_cast<SampleType>(1.0) - dt) {
            t = (t - static_cast<SampleType>(1.0)) / dt;
            return t * t + t + t + static_cast<SampleType>(1.0);
        } else {
            return static_cast<SampleType>(0.0);
        }
    }

    template <FloatType SampleType>
    [[nodiscard]] SampleType blamp(SampleType t, SampleType phaseIncrement) noexcept {
        const auto dt = phaseIncrement;
        if (t < dt) {
            t = t / dt - static_cast<SampleType>(1.0);
            return static_cast<SampleType>(1.0) / static_cast<SampleType>(3.0) * std::pow(t, static_cast<SampleType>(3.0));
        } else if (t > static_cast<SampleType>(1.0) - dt) {
            t = (t - static_cast<SampleType>(1.0)) / dt + static_cast<SampleType>(1.0);
            return static_cast<SampleType>(1.0) / static_cast<SampleType>(3.0) * std::pow(t, static_cast<SampleType>(3.0));
        } else {
            return static_cast<SampleType>(0.0);
        }
    }

    template <FloatType SampleType>
    void OscillatorBase<SampleType>::initialise(double sampleRate) {
        m_sampleRate = sampleRate;
        reset();
    }

    template <FloatType SampleType>
    void OscillatorBase<SampleType>::setFrequency(SampleType newFrequency) noexcept {
        const auto period = static_cast<SampleType>(1.0f) / newFrequency;
        const auto periodSamples = period * static_cast<SampleType>(m_sampleRate);
        const auto incr = static_cast<SampleType>(1.0f) / periodSamples;
        m_phaseIncrement = incr;
        // m_phaseIncrement = newFrequency / static_cast<SampleType>(m_sampleRate);
    }

    template <FloatType SampleType>
    void OscillatorBase<SampleType>::setPhaseOffset(SampleType newPhaseOffset) noexcept {
        m_phaseOffset = newPhaseOffset;
        reset();
    }

    template <FloatType SampleType>
    void OscillatorBase<SampleType>::reset() noexcept {
        m_phase = m_phaseOffset;
    }

    template <FloatType SampleType>
    void OscillatorBase<SampleType>::incrementPhase() noexcept {
        m_phase += m_phaseIncrement;
        while (m_phase >= static_cast<SampleType>(1.0)) {
            m_phase -= static_cast<SampleType>(1.0);
        }
    }

    template <FloatType SampleType>
    SampleType SineOscillator<SampleType>::operator()() noexcept {
        const auto x = operator()(this->m_phase);
        this->incrementPhase();
        return x;
    }

    template <FloatType SampleType>
    SampleType SineOscillator<SampleType>::operator()(SampleType phase) noexcept {
        const auto x = std::sin(phase * (static_cast<SampleType>(2.0) * std::numbers::pi_v<SampleType>));
        return x;
    }

    template <FloatType SampleType, Bandlimiting Blep>
    SampleType TriOscillator<SampleType, Blep>::operator()() noexcept {
        const auto res = operator()(this->m_phase);
        this->incrementPhase();
        return res;
    }

    template <FloatType SampleType, Bandlimiting Blep>
    SampleType TriOscillator<SampleType, Blep>::operator()(SampleType phase) noexcept {
        auto x = static_cast<SampleType>(4.0) * std::abs(phase - std::floor(phase + static_cast<SampleType>(0.75)) + static_cast<SampleType>(0.25)) - static_cast<SampleType>(1.0);
        if constexpr (Blep == Bandlimiting::On) {
            const auto t1 = std::fmod(phase + static_cast<SampleType>(0.25), static_cast<SampleType>(1.0));
            const auto t2 = std::fmod(phase + static_cast<SampleType>(0.75), static_cast<SampleType>(1.0));
            const auto b1 = blamp(t1, this->m_phaseIncrement);
            const auto b2 = blamp(t2, this->m_phaseIncrement);
            const auto delta = b1 - b2;
            const auto blamped = static_cast<SampleType>(4.0) * this->m_phaseIncrement * delta;
            x += blamped;
        }
        return x;
    }


    template <FloatType SampleType, Bandlimiting Blep>
    SampleType SawOscillator<SampleType, Blep>::operator()() noexcept {
        const auto x = operator()(this->m_phase);
        this->incrementPhase();
        return x;
    }

    template <FloatType SampleType, Bandlimiting Blep>
    SampleType SawOscillator<SampleType, Blep>::operator()(SampleType phase) noexcept {
        auto x = (static_cast<SampleType>(2.0) * phase) - static_cast<SampleType>(1.0);
        if constexpr (Blep == Bandlimiting::On) {
            x -= polyBlep(phase, this->m_phaseIncrement);
        }
        return x;
    }


    template <FloatType SampleType, Bandlimiting Blep>
    SampleType SquareOscillator<SampleType, Blep>::operator()() noexcept {
        const auto value = operator()(this->m_phase);
        this->incrementPhase();
        return value;
    }

    template <FloatType SampleType, Bandlimiting Blep>
    SampleType SquareOscillator<SampleType, Blep>::operator()(SampleType phase) noexcept {
        SampleType value;
        if (phase < static_cast<SampleType>(0.5)) {
            value = static_cast<SampleType>(1.0);
        } else {
            value = static_cast<SampleType>(-1.0);
        }
        if constexpr (Blep == Bandlimiting::On) {
            value += polyBlep(phase, this->m_phaseIncrement);
            value -= polyBlep(std::fmod(phase + static_cast<SampleType>(0.5), static_cast<SampleType>(1.0)), this->m_phaseIncrement);
        }
        return value;
    }

    template <FloatType SampleType, Bandlimiting Blep>
    SampleType PulseOscillator<SampleType, Blep>::operator()() noexcept {
        const auto value = operator()(this->m_phase);
        this->incrementPhase();
        return value;
    }

    template <FloatType SampleType, Bandlimiting Blep>
    SampleType PulseOscillator<SampleType, Blep>::operator()(SampleType phase) noexcept {
        SampleType value;
        if (phase < m_pulsewidth) {
            value = static_cast<SampleType>(1.0);
        } else {
            value = static_cast<SampleType>(-1.0);
        }
        if constexpr (Blep == Bandlimiting::On) {
            value += polyBlep(phase, this->m_phaseIncrement);
            value -= polyBlep(std::fmod(phase + static_cast<SampleType>(m_pulsewidth), static_cast<SampleType>(1.0)), this->m_phaseIncrement);
        }
        return value;
    }

    template <FloatType SampleType, Bandlimiting Blep>
    void PulseOscillator<SampleType, Blep>::setPulsewidth(SampleType newPulsewidth) noexcept {
        m_pulsewidth = newPulsewidth;
    }

    template <FloatType SampleType>
    NoiseOscillator<SampleType>::NoiseOscillator(std::random_device& rd) : m_rng(rd) {
    }

    template <FloatType SampleType>
    SampleType NoiseOscillator<SampleType>::operator()() noexcept {
        const auto random = m_rng.generate(utils::Range<SampleType>{ static_cast<SampleType>(-1.0), static_cast<SampleType>(1.0) });
        return random;
    }

    template <FloatType SampleType>
    SampleType NoiseOscillator<SampleType>::operator()(SampleType /*phase*/) noexcept {
        return operator()();
    }

    template <FloatType SampleType, Bandlimiting Blep>
    MultiOscillator<SampleType, Blep>::MultiOscillator(std::random_device& rd) : m_shape(Shape::Sine),
                                                                                 m_noise(rd) {
    }

    template <FloatType SampleType, Bandlimiting Blep>
    MultiOscillator<SampleType, Blep>::MultiOscillator(std::random_device& rd, Shape shape) : m_shape(shape),
                                                                                              m_noise(rd) {
    }

    template <FloatType SampleType, Bandlimiting Blep>
    void MultiOscillator<SampleType, Blep>::initialise(double sampleRate) {
        m_sampleRate = sampleRate;
        m_sine.initialise(sampleRate);
        m_tri.initialise(sampleRate);
        m_saw.initialise(sampleRate);
        m_square.initialise(sampleRate);
        m_pulse.initialise(sampleRate);
        m_noise.initialise(sampleRate);
    }

    template <FloatType SampleType, Bandlimiting Blep>
    SampleType MultiOscillator<SampleType, Blep>::operator()() noexcept {
        SampleType v{ static_cast<SampleType>(0.0) };
        switch (m_shape) {
            case Shape::Sine: {
                v = m_sine(m_phase);
                break;
            }
            case Shape::Triangle: {
                v = m_tri(m_phase);
                break;
            }
            case Shape::Saw: {
                v = m_saw(m_phase);
                break;
            }
            case Shape::Square: {
                v = m_square(m_phase);
                break;
            }
            case Shape::Pulse: {
                v = m_pulse(m_phase);
                break;
            }
            case Shape::Noise: {
                v = m_noise(m_phase);
                break;
            }
            default: break;
        }
        incrementPhase();
        return v;
    }

    template <FloatType SampleType, Bandlimiting Blep>
    void MultiOscillator<SampleType, Blep>::reset() noexcept {
        m_phase = m_phaseOffset;
        m_sine.reset();
        m_tri.reset();
        m_saw.reset();
        m_square.reset();
        m_pulse.reset();
        m_noise.reset();
    }

    template <FloatType SampleType, Bandlimiting Blep>
    void MultiOscillator<SampleType, Blep>::setShape(Shape shape) {
        m_shape = shape;
    }

    template <FloatType SampleType, Bandlimiting Blep>
    void MultiOscillator<SampleType, Blep>::setFrequency(SampleType newFrequency) noexcept {
        m_phaseIncrement = newFrequency / static_cast<SampleType>(m_sampleRate);
        // TODO: I think we can delete these?
        m_sine.setFrequency(newFrequency);
        m_tri.setFrequency(newFrequency);
        m_saw.setFrequency(newFrequency);
        m_square.setFrequency(newFrequency);
        m_pulse.setFrequency(newFrequency);
        m_noise.setFrequency(newFrequency);
    }

    template <FloatType SampleType, Bandlimiting Blep>
    void MultiOscillator<SampleType, Blep>::setPhaseOffset(SampleType newPhaseOffset) noexcept {
        m_phaseOffset = newPhaseOffset;
        reset();
    }

    template <FloatType SampleType, Bandlimiting Blep>
    void MultiOscillator<SampleType, Blep>::setPulsewidth(SampleType newPulsewidth) noexcept {
        m_pulse.setPulsewidth(newPulsewidth);
    }

    template <FloatType SampleType, Bandlimiting Blep>
    void MultiOscillator<SampleType, Blep>::incrementPhase() noexcept {
        m_phase += m_phaseIncrement;
        while (m_phase >= static_cast<SampleType>(1.0)) {
            m_phase -= static_cast<SampleType>(1.0);
        }
    }


    template class OscillatorBase<float>;
    template class OscillatorBase<double>;
    template class SineOscillator<float>;
    template class SineOscillator<double>;
    template class TriOscillator<float, Bandlimiting::Off>;
    template class TriOscillator<float, Bandlimiting::On>;
    template class TriOscillator<double, Bandlimiting::Off>;
    template class TriOscillator<double, Bandlimiting::On>;
    template class SawOscillator<float, Bandlimiting::Off>;
    template class SawOscillator<float, Bandlimiting::On>;
    template class SawOscillator<double, Bandlimiting::Off>;
    template class SawOscillator<double, Bandlimiting::On>;
    template class SquareOscillator<float, Bandlimiting::Off>;
    template class SquareOscillator<float, Bandlimiting::On>;
    template class SquareOscillator<double, Bandlimiting::Off>;
    template class SquareOscillator<double, Bandlimiting::On>;
    template class PulseOscillator<float, Bandlimiting::Off>;
    template class PulseOscillator<float, Bandlimiting::On>;
    template class PulseOscillator<double, Bandlimiting::Off>;
    template class PulseOscillator<double, Bandlimiting::On>;
    template class NoiseOscillator<float>;
    template class NoiseOscillator<double>;
    template class MultiOscillator<float, Bandlimiting::Off>;
    template class MultiOscillator<float, Bandlimiting::On>;
} // namespace marvin::dsp::oscillators
