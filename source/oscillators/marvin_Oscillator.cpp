// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/oscillators/marvin_Oscillator.h"
#include <numbers>
#include <cmath>
#include <random>
#include "marvin/library/enable_warnings.h"

namespace marvin::oscillators {
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
    void OscillatorBase<SampleType>::initialise(double sampleRate) {
        m_sampleRate = sampleRate;
        reset();
    }

    template <FloatType SampleType>
    void OscillatorBase<SampleType>::setFrequency(SampleType newFrequency) noexcept {
        m_phaseIncrement = newFrequency / static_cast<SampleType>(m_sampleRate);
    }

    template <FloatType SampleType>
    void OscillatorBase<SampleType>::reset() noexcept {
        m_phase = static_cast<SampleType>(0.0);
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

    template <FloatType SampleType, BlepType Blep>
    SampleType TriOscillator<SampleType, Blep>::operator()() noexcept {
        const auto res = operator()(this->m_phase);
        this->incrementPhase();
        return res;
    }

    template <FloatType SampleType, BlepType Blep>
    SampleType TriOscillator<SampleType, Blep>::operator()(SampleType phase) noexcept {
        SampleType x;
        if constexpr (std::is_same_v<Blep, BlepState::Off>) {
            x = static_cast<SampleType>(-1.0) + (static_cast<SampleType>(2.0) * phase);
            x = static_cast<SampleType>(2.0) * (std::fabs(x) - static_cast<SampleType>(0.5));
        } else {
            // Leaky Integrated polyblepped Square
            if (phase < static_cast<SampleType>(0.5)) {
                x = static_cast<SampleType>(1.0);
            } else {
                x = static_cast<SampleType>(-1.0);
            }
            x += polyBlep(phase, this->m_phaseIncrement);
            x -= polyBlep(std::fmod(phase + static_cast<SampleType>(0.5), static_cast<SampleType>(1.0)), this->m_phaseIncrement);
            x = m_integrator(x, this->m_phaseIncrement);
        }
        return x;
    }


    template <FloatType SampleType, BlepType Blep>
    SampleType SawOscillator<SampleType, Blep>::operator()() noexcept {
        const auto x = operator()(this->m_phase);
        this->incrementPhase();
        return x;
    }

    template <FloatType SampleType, BlepType Blep>
    SampleType SawOscillator<SampleType, Blep>::operator()(SampleType phase) noexcept {
        auto x = (static_cast<SampleType>(2.0) * phase) - static_cast<SampleType>(1.0);
        if constexpr (std::is_same_v<Blep, BlepState::On>) {
            x -= polyBlep(phase, this->m_phaseIncrement);
        }
        return x;
    }


    template <FloatType SampleType, BlepType Blep>
    SampleType SquareOscillator<SampleType, Blep>::operator()() noexcept {
        const auto value = operator()(this->m_phase);
        this->incrementPhase();
        return value;
    }

    template <FloatType SampleType, BlepType Blep>
    SampleType SquareOscillator<SampleType, Blep>::operator()(SampleType phase) noexcept {
        SampleType value;
        if (phase < static_cast<SampleType>(0.5)) {
            value = static_cast<SampleType>(1.0);
        } else {
            value = static_cast<SampleType>(-1.0);
        }
        if constexpr (std::is_same_v<Blep, BlepState::On>) {
            value += polyBlep(phase, this->m_phaseIncrement);
            value -= polyBlep(std::fmod(phase + static_cast<SampleType>(0.5), static_cast<SampleType>(1.0)), this->m_phaseIncrement);
        }
        return value;
    }

    template <FloatType SampleType, BlepType Blep>
    SampleType PulseOscillator<SampleType, Blep>::operator()() noexcept {
        const auto value = operator()(this->m_phase);
        this->incrementPhase();
        return value;
    }

    template <FloatType SampleType, BlepType Blep>
    SampleType PulseOscillator<SampleType, Blep>::operator()(SampleType phase) noexcept {
        SampleType value;
        if (phase < m_pulsewidth) {
            value = static_cast<SampleType>(1.0);
        } else {
            value = static_cast<SampleType>(-1.0);
        }
        if constexpr (std::is_same_v<Blep, BlepState::On>) {
            value += polyBlep(phase, this->m_phaseIncrement);
            value -= polyBlep(std::fmod(phase + static_cast<SampleType>(m_pulsewidth), static_cast<SampleType>(1.0)), this->m_phaseIncrement);
        }
        return value;
    }

    template <FloatType SampleType, BlepType Blep>
    void PulseOscillator<SampleType, Blep>::setPulsewidth(SampleType newPulsewidth) noexcept {
        m_pulsewidth = newPulsewidth;
    }

    template <FloatType SampleType>
    NoiseOscillator<SampleType>::NoiseOscillator(std::random_device& rd) : m_rng(rd) {
    }

    template <FloatType SampleType>
    SampleType NoiseOscillator<SampleType>::operator()() noexcept {
        const auto random = m_rng.generate(utils::Random::Range<SampleType>{ static_cast<SampleType>(-1.0), static_cast<SampleType>(1.0) });
        return random;
    }

    template <FloatType SampleType>
    SampleType NoiseOscillator<SampleType>::operator()(SampleType /*phase*/) noexcept {
        return operator()();
    }

    template <FloatType SampleType, BlepType Blep>
    MultiOscillator<SampleType, Blep>::MultiOscillator(std::random_device& rd) : m_shape(SHAPE::SINE),
                                                                                 m_noise(rd) {
    }

    template <FloatType SampleType, BlepType Blep>
    MultiOscillator<SampleType, Blep>::MultiOscillator(std::random_device& rd, SHAPE shape) : m_shape(shape),
                                                                                              m_noise(rd) {
    }

    template <FloatType SampleType, BlepType Blep>
    void MultiOscillator<SampleType, Blep>::initialise(double sampleRate) {
        m_sampleRate = sampleRate;
        m_sine.initialise(sampleRate);
        m_tri.initialise(sampleRate);
        m_saw.initialise(sampleRate);
        m_square.initialise(sampleRate);
        m_pulse.initialise(sampleRate);
        m_noise.initialise(sampleRate);
    }

    template <FloatType SampleType, BlepType Blep>
    SampleType MultiOscillator<SampleType, Blep>::operator()() noexcept {
        SampleType v{ static_cast<SampleType>(0.0) };
        switch (m_shape) {
            case SHAPE::SINE: {
                v = m_sine(m_phase);
                break;
            }
            case SHAPE::TRIANGLE: {
                v = m_tri(m_phase);
                break;
            }
            case SHAPE::SAW: {
                v = m_saw(m_phase);
                break;
            }
            case SHAPE::SQUARE: {
                v = m_square(m_phase);
                break;
            }
            case SHAPE::PULSE: {
                v = m_pulse(m_phase);
                break;
            }
            case SHAPE::NOISE: {
                v = m_noise(m_phase);
                break;
            }
            default: break;
        }
        incrementPhase();
        return v;
    }

    template <FloatType SampleType, BlepType Blep>
    void MultiOscillator<SampleType, Blep>::reset() noexcept {
        m_phase = static_cast<SampleType>(0.0);
        m_sine.reset();
        m_tri.reset();
        m_saw.reset();
        m_square.reset();
        m_pulse.reset();
        m_noise.reset();
    }

    template <FloatType SampleType, BlepType Blep>
    void MultiOscillator<SampleType, Blep>::setShape(SHAPE shape) {
        m_shape = shape;
    }

    template <FloatType SampleType, BlepType Blep>
    void MultiOscillator<SampleType, Blep>::setFrequency(SampleType newFrequency) noexcept {
        m_phaseIncrement = newFrequency / static_cast<SampleType>(m_sampleRate);
        m_sine.setFrequency(newFrequency);
        m_tri.setFrequency(newFrequency);
        m_saw.setFrequency(newFrequency);
        m_square.setFrequency(newFrequency);
        m_pulse.setFrequency(newFrequency);
        m_noise.setFrequency(newFrequency);
    }

    template <FloatType SampleType, BlepType Blep>
    void MultiOscillator<SampleType, Blep>::setPulsewidth(SampleType newPulsewidth) noexcept {
        m_pulse.setPulsewidth(newPulsewidth);
    }

    template <FloatType SampleType, BlepType Blep>
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
    template class TriOscillator<float, BlepState::Off>;
    template class TriOscillator<float, BlepState::On>;
    template class TriOscillator<double, BlepState::Off>;
    template class TriOscillator<double, BlepState::On>;
    template class SawOscillator<float, BlepState::Off>;
    template class SawOscillator<float, BlepState::On>;
    template class SawOscillator<double, BlepState::Off>;
    template class SawOscillator<double, BlepState::On>;
    template class SquareOscillator<float, BlepState::Off>;
    template class SquareOscillator<float, BlepState::On>;
    template class SquareOscillator<double, BlepState::Off>;
    template class SquareOscillator<double, BlepState::On>;
    template class PulseOscillator<float, BlepState::Off>;
    template class PulseOscillator<float, BlepState::On>;
    template class PulseOscillator<double, BlepState::Off>;
    template class PulseOscillator<double, BlepState::On>;
    template class NoiseOscillator<float>;
    template class NoiseOscillator<double>;
    template class MultiOscillator<float, BlepState::Off>;
    template class MultiOscillator<float, BlepState::On>;
} // namespace marvin::oscillators
#include "marvin/library/disable_warnings.h"
