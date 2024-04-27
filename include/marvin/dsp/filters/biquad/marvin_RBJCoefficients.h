// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_RBJCOEFFICIENTS_H
#define MARVIN_RBJCOEFFICIENTS_H
#include "marvin/dsp/filters/biquad/marvin_BiquadCoefficients.h"
#include <cmath>
#include <numbers>
namespace marvin::dsp::filters {
    template <FloatType SampleType>
    /**
        RBJ Lowpass implementation.
    */
    [[nodiscard]] BiquadCoefficients<SampleType> lowpass(double sampleRate, SampleType cutoff, SampleType q) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto omega{ twoPi * (cutoff / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto alpha{ sinOmega / (static_cast<SampleType>(2.0) * q) };
        const auto b0 = (static_cast<SampleType>(1.0) - cosOmega) / static_cast<SampleType>(2.0);
        const auto b1 = static_cast<SampleType>(1.0) - cosOmega;
        const auto b2 = b0;
        const auto a0 = static_cast<SampleType>(1.0) + alpha;
        const auto a1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto a2 = static_cast<SampleType>(1.0) - alpha;
        return {
            .b0 = b0,
            .b1 = b1,
            .b2 = b2,
            .a0 = a0,
            .a1 = a1,
            .a2 = a2
        };
    }

    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> highpass(double sampleRate, SampleType cutoff, SampleType q) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto omega{ twoPi * (cutoff / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto alpha{ sinOmega / (static_cast<SampleType>(2.0) * q) };
        const auto b0 = (static_cast<SampleType>(1.0) + cosOmega) / static_cast<SampleType>(2.0);
        const auto b1 = static_cast<SampleType>(-1.0) * (static_cast<SampleType>(1.0) + cosOmega);
        const auto b2 = b0;
        const auto a0 = static_cast<SampleType>(1.0) + alpha;
        const auto a1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto a2 = static_cast<SampleType>(1.0) - alpha;
        return {
            .b0 = b0,
            .b1 = b1,
            .b2 = b2,
            .a0 = a0,
            .a1 = a1,
            .a2 = a2
        };
    }

    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> bandpass(double sampleRate, SampleType centreFrequency, SampleType bandwidth, SampleType peakGain) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto omega{ twoPi * (centreFrequency / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto log2Over2 = std::log(static_cast<SampleType>(2.0)) / static_cast<SampleType>(2.0);
        const auto alpha = sinOmega * sinh(log2Over2 * bandwidth * (omega / sinOmega));
        const auto b0 = (sinOmega / 2.0f) * (peakGain * alpha);
        const auto b1 = 0.0f;
        const auto b2 = -1.0f * (sinOmega / 2.0f);
        const auto a0 = 1.0f + alpha;
        const auto a1 = -2.0f * cosOmega;
        const auto a2 = 1.0f - alpha;
        return {
            .b0 = b0,
            .b1 = b1,
            .b2 = b2,
            .a0 = a0,
            .a1 = a1,
            .a2 = a2
        };
    }

    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> bandpass(double sampleRate, SampleType centreFrequency, SampleType bandwidth) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto omega{ twoPi * (centreFrequency / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto log2Over2 = std::log(static_cast<SampleType>(2.0)) / static_cast<SampleType>(2.0);
        const auto alpha = sinOmega * sinh(log2Over2 * bandwidth * (omega / sinOmega));
        const auto b0 = alpha;
        const auto b1 = static_cast<SampleType>(0.0);
        const auto b2 = static_cast<SampleType>(-1.0) * alpha;
        const auto a0 = static_cast<SampleType>(1.0) + alpha;
        const auto a1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto a2 = static_cast<SampleType>(1.0) - alpha;
        return {
            .b0 = b0,
            .b1 = b1,
            .b2 = b2,
            .a0 = a0,
            .a1 = a1,
            .a2 = a2
        };
    }

    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> notch(double sampleRate, SampleType centreFrequency, SampleType bandwidth) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto ffs{ static_cast<float>(sampleRate) };
        const auto omega{ twoPi * (centreFrequency / ffs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto log2Over2 = std::log(static_cast<SampleType>(2.0)) / static_cast<SampleType>(2.0);
        const auto alpha = sinOmega * sinh(log2Over2 * bandwidth * (omega / sinOmega));
        const auto b0 = static_cast<SampleType>(1.0);
        const auto b1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto b2 = static_cast<SampleType>(1.0);
        const auto a0 = static_cast<SampleType>(1.0) + alpha;
        const auto a1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto a2 = static_cast<SampleType>(1.0) - alpha;
        return {
            .b0 = b0,
            .b1 = b1,
            .b2 = b2,
            .a0 = a0,
            .a1 = a1,
            .a2 = a2
        };
    }
    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> allpass(double sampleRate, SampleType cutoff, SampleType bandwidth) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto omega{ twoPi * (cutoff / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto log2Over2 = std::log(static_cast<SampleType>(2.0)) / static_cast<SampleType>(2.0);
        const auto alpha = sinOmega * sinh(log2Over2 * bandwidth * (omega / sinOmega));
        const auto b0 = static_cast<SampleType>(1.0) - alpha;
        const auto b1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto b2 = static_cast<SampleType>(1.0) + alpha;
        const auto a0 = static_cast<SampleType>(1.0) + alpha;
        const auto a1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto a2 = static_cast<SampleType>(1.0) - alpha;
        return {
            .b0 = b0,
            .b1 = b1,
            .b2 = b2,
            .a0 = a0,
            .a1 = a1,
            .a2 = a2
        };
    }
    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> peak(double sampleRate, SampleType centreFrequency, SampleType bandwidth, SampleType dbGain) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto A = std::pow(static_cast<SampleType>(10.0), dbGain / static_cast<SampleType>(40.0));
        const auto omega{ twoPi * (centreFrequency / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto log2Over2 = std::log(static_cast<SampleType>(2.0)) / static_cast<SampleType>(2.0);
        const auto alpha = sinOmega * sinh(log2Over2 * bandwidth * (omega / sinOmega));
        const auto b0 = static_cast<SampleType>(1.0) + (alpha * A);
        const auto b1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto b2 = static_cast<SampleType>(1.0) - (alpha * A);
        const auto a0 = static_cast<SampleType>(1.0) + (alpha / A);
        const auto a1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto a2 = static_cast<SampleType>(1.0) - (alpha / A);
        return {
            .b0 = b0,
            .b1 = b1,
            .b2 = b2,
            .a0 = a0,
            .a1 = a1,
            .a2 = a2
        };
    }
    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> lowShelf(double sampleRate, SampleType centreFrequency, SampleType slope, SampleType dbGain) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto A = std::pow(static_cast<SampleType>(10.0), dbGain / static_cast<SampleType>(40.0));
        const auto omega{ twoPi * (centreFrequency / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto alpha = (sinOmega / static_cast<SampleType>(2.0)) * std::sqrt(((A * (static_cast<SampleType>(1.0) / A)) * ((static_cast<SampleType>(1.0) / slope) - static_cast<SampleType>(1.0))) + static_cast<SampleType>(2.0));
        const auto twoRootAAlpha = static_cast<SampleType>(2.0) * std::sqrt(A * alpha);
        const auto b0 = A * ((A + static_cast<SampleType>(1.0)) - (A - static_cast<SampleType>(1.0)) * cosOmega + twoRootAAlpha);
        const auto b1 = static_cast<SampleType>(2.0) * A * ((A - static_cast<SampleType>(1.0)) - (A + static_cast<SampleType>(1.0)) * cosOmega);
        const auto b2 = A * ((A + static_cast<SampleType>(1.0)) - (A - static_cast<SampleType>(1.0)) * cosOmega - twoRootAAlpha);
        const auto a0 = (A + static_cast<SampleType>(1.0)) + (A - static_cast<SampleType>(1.0)) * cosOmega + twoRootAAlpha;
        const auto a1 = static_cast<SampleType>(-2.0) * ((A - static_cast<SampleType>(1.0)) + (A + static_cast<SampleType>(1.0)) * cosOmega);
        const auto a2 = (A + static_cast<SampleType>(1.0)) + (A - static_cast<SampleType>(1.0)) * cosOmega - twoRootAAlpha;
        return {
            .b0 = b0,
            .b1 = b1,
            .b2 = b2,
            .a0 = a0,
            .a1 = a1,
            .a2 = a2
        };
    }

    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> highShelf(double sampleRate, SampleType centreFrequency, SampleType slope, SampleType dbGain) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto A = std::pow(static_cast<SampleType>(10.0), dbGain / static_cast<SampleType>(40.0));
        const auto omega{ twoPi * (centreFrequency / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto alpha = (sinOmega / static_cast<SampleType>(2.0)) * std::sqrt(((A * (static_cast<SampleType>(1.0) / A)) * ((static_cast<SampleType>(1.0) / slope) - static_cast<SampleType>(1.0))) + static_cast<SampleType>(2.0));
        const auto twoRootAAlpha = static_cast<SampleType>(2.0) * std::sqrt(A * alpha);
        const auto b0 = A * ((A + static_cast<SampleType>(1.0)) + (A - static_cast<SampleType>(1.0)) * cosOmega + twoRootAAlpha);
        const auto b1 = static_cast<SampleType>(-2.0) * A * ((A - static_cast<SampleType>(1.0)) + (A + static_cast<SampleType>(1.0)) * cosOmega);
        const auto b2 = A * ((A + static_cast<SampleType>(1.0)) + (A - static_cast<SampleType>(1.0)) * cosOmega - twoRootAAlpha);
        const auto a0 = (A + static_cast<SampleType>(1.0)) - (A - static_cast<SampleType>(1.0)) * cosOmega + twoRootAAlpha;
        const auto a1 = static_cast<SampleType>(2.0) * ((A - static_cast<SampleType>(1.0)) - (A + static_cast<SampleType>(1.0)) * cosOmega);
        const auto a2 = (A + static_cast<SampleType>(1.0)) - (A - static_cast<SampleType>(1.0)) * cosOmega - twoRootAAlpha;
        return {
            .b0 = b0,
            .b1 = b1,
            .b2 = b2,
            .a0 = a0,
            .a1 = a1,
            .a2 = a2
        };
    }
} // namespace marvin::dsp::filters


#endif