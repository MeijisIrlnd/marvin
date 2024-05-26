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
namespace marvin::dsp::filters::rbj {
    /**
        An RBJ lowpass implementation for use with the Biquad class, with formulae from the [RBJ Cookbook](https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html)
        \param sampleRate The sample rate to base the calculations off.
        \param cutoff The frequency of the lowpass
        \param q The resonance of the lowpass(between 0 and 1)
        \return An instance of BiquadCoefficients populated with the resulting coefficients.* /
        */
    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> lowpass(double sampleRate, SampleType cutoff, SampleType q) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto omega{ twoPi * (cutoff / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto alpha{ sinOmega / (static_cast<SampleType>(2.0) * q) };
        const auto a0 = (static_cast<SampleType>(1.0) - cosOmega) / static_cast<SampleType>(2.0);
        const auto a1 = static_cast<SampleType>(1.0) - cosOmega;
        const auto a2 = a0;
        const auto b0 = static_cast<SampleType>(1.0) + alpha;
        const auto b1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto b2 = static_cast<SampleType>(1.0) - alpha;
        return BiquadCoefficients<SampleType>{
            .a0 = a0,
            .a1 = a1,
            .a2 = a2,
            .b0 = b0,
            .b1 = b1,
            .b2 = b2
        };
    }

    /**
        An RBJ highpass implementation for use with the Biquad class, based on the formulae from the RBJ Cookbook (https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html)
        \param sampleRate The sample rate to base the calculations off.
        \param cutoff The frequency of the highpass
        \param q The resonance of the highpass (between 0 and 1)
        \return An instance of BiquadCoefficients populated with the resulting coefficients.
    */
    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> highpass(double sampleRate, SampleType cutoff, SampleType q) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto omega{ twoPi * (cutoff / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto alpha{ sinOmega / (static_cast<SampleType>(2.0) * q) };
        const auto a0 = (static_cast<SampleType>(1.0) + cosOmega) / static_cast<SampleType>(2.0);
        const auto a1 = static_cast<SampleType>(-1.0) * (static_cast<SampleType>(1.0) + cosOmega);
        const auto a2 = a0;
        const auto b0 = static_cast<SampleType>(1.0) + alpha;
        const auto b1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto b2 = static_cast<SampleType>(1.0) - alpha;
        return BiquadCoefficients<SampleType>{
            .a0 = a0,
            .a1 = a1,
            .a2 = a2,
            .b0 = b0,
            .b1 = b1,
            .b2 = b2
        };
    }

    /**
        A constant skirt-gain RBJ bandpass implementation for use with the Biquad class, based on the formulae from the RBJ Cookbook (https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html)
        \param sampleRate The sample rate to base the calculations off.
        \param centreFrequency The centre frequency of the bandpass.
        \param peakGain The peak gain of the bandpass (between 0 and 1)
        \return An instance of BiquadCoefficients populated with the resulting coefficients.
    */
    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> bandpass(double sampleRate, SampleType centreFrequency, SampleType bandwidth, SampleType peakGain) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto omega{ twoPi * (centreFrequency / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto log2Over2 = std::log(static_cast<SampleType>(2.0)) / static_cast<SampleType>(2.0);
        const auto alpha = sinOmega * sinh(log2Over2 * bandwidth * (omega / sinOmega));
        const auto a0 = (sinOmega / static_cast<SampleType>(2.0)) * (peakGain * alpha);
        const auto a1 = static_cast<SampleType>(0.0);
        const auto a2 = static_cast<SampleType>(-1.0) * (sinOmega / static_cast<SampleType>(2.0));
        const auto b0 = static_cast<SampleType>(1.0) + alpha;
        const auto b1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto b2 = static_cast<SampleType>(1.0) - alpha;
        return BiquadCoefficients<SampleType>{
            .a0 = a0,
            .a1 = a1,
            .a2 = a2,
            .b0 = b0,
            .b1 = b1,
            .b2 = b2
        };
    }

    /**
        A constant 0db peak-gain RBJ bandpass implementation for use with the Biquad class, based on the formulae from the RBJ Cookbook (https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html)
        \param sampleRate The sample rate to base the calculations off.
        \param centreFrequency The centre frequency of the bandpass.
        \param bandwidth The bandwidth of the bandpass in octaves.
        \return An instance of BiquadCoefficients populated with the resulting coefficients.
    */
    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> bandpass(double sampleRate, SampleType centreFrequency, SampleType bandwidth) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto omega{ twoPi * (centreFrequency / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto log2Over2 = std::log(static_cast<SampleType>(2.0)) / static_cast<SampleType>(2.0);
        const auto alpha = sinOmega * sinh(log2Over2 * bandwidth * (omega / sinOmega));
        const auto a0 = alpha;
        const auto a1 = static_cast<SampleType>(0.0);
        const auto a2 = static_cast<SampleType>(-1.0) * alpha;
        const auto b0 = static_cast<SampleType>(1.0) + alpha;
        const auto b1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto b2 = static_cast<SampleType>(1.0) - alpha;
        return BiquadCoefficients<SampleType>{
            .a0 = a0,
            .a1 = a1,
            .a2 = a2,
            .b0 = b0,
            .b1 = b1,
            .b2 = b2
        };
    }

    /**
        An RBJ notch implementation for use with the Biquad class, based on the formulae from the RBJ Cookbook (https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html)
        \param sampleRate The sample rate to base the calculations off.
        \param centreFrequency The centre frequency of the notch.
        \param bandwidth The bandwidth of the notch in octaves.
        \return An instance of BiquadCoefficients populated with the resulting coefficients.
    */
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
        const auto a0 = static_cast<SampleType>(1.0);
        const auto a1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto a2 = static_cast<SampleType>(1.0);
        const auto b0 = static_cast<SampleType>(1.0) + alpha;
        const auto b1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto b2 = static_cast<SampleType>(1.0) - alpha;
        return BiquadCoefficients<SampleType>{
            .a0 = a0,
            .a1 = a1,
            .a2 = a2,
            .b0 = b0,
            .b1 = b1,
            .b2 = b2
        };
    }
    /**
        An RBJ allpass implementation for use with the Biquad class, based on the formulae from the RBJ Cookbook (https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html)
        \param sampleRate The sample rate to base the calculations off.
        \param cutoff The centre frequency of the allpass.
        \param q The resonance of the allpass (0 to 1).
        \return An instance of BiquadCoefficients populated with the resulting coefficients.
    */
    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> allpass(double sampleRate, SampleType cutoff, SampleType q) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto omega{ twoPi * (cutoff / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto log2Over2 = std::log(static_cast<SampleType>(2.0)) / static_cast<SampleType>(2.0);
        const auto alpha = sinOmega * sinh(log2Over2 * q * (omega / sinOmega));
        const auto a0 = static_cast<SampleType>(1.0) - alpha;
        const auto a1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto a2 = static_cast<SampleType>(1.0) + alpha;
        const auto b0 = static_cast<SampleType>(1.0) + alpha;
        const auto b1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto b2 = static_cast<SampleType>(1.0) - alpha;
        return BiquadCoefficients<SampleType>{
            .a0 = a0,
            .a1 = a1,
            .a2 = a2,
            .b0 = b0,
            .b1 = b1,
            .b2 = b2
        };
    }

    /**
        An RBJ bell implementation for use with the Biquad class, based on the formulae from the RBJ Cookbook (https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html)
        \param sampleRate The sample rate to base the calculations off.
        \param centreFrequency The centre frequency of the bell.
        \param bandwidth The bandwidth of the bell in octaves.
        \param dbGain The gain of the bell, in decibels.
        \return An instance of BiquadCoefficients populated with the resulting coefficients.
    */
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
        const auto a0 = static_cast<SampleType>(1.0) + (alpha * A);
        const auto a1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto a2 = static_cast<SampleType>(1.0) - (alpha * A);
        const auto b0 = static_cast<SampleType>(1.0) + (alpha / A);
        const auto b1 = static_cast<SampleType>(-2.0) * cosOmega;
        const auto b2 = static_cast<SampleType>(1.0) - (alpha / A);
        return BiquadCoefficients<SampleType>{
            .a0 = a0,
            .a1 = a1,
            .a2 = a2,
            .b0 = b0,
            .b1 = b1,
            .b2 = b2
        };
    }

    /**
        An RBJ low shelf implementation for use with the Biquad class, based on the formulae from the RBJ Cookbook (https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html)
        \param sampleRate The sample rate to base the calculations off.
        \param centreFrequency The centre frequency of the shelf.
        \param slope The slope of the shelf in db/octave. At slope=1, the shelf is as steep as it can be. Should be between `std::numeric_limits<SampleType>::min()`, and 1`.
        \param dbGain The gain of the shelf, in decibels.
        \return An instance of BiquadCoefficients populated with the resulting coefficients.
    */
    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> lowShelf(double sampleRate, SampleType centreFrequency, SampleType slope, SampleType dbGain) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto A = std::pow(static_cast<SampleType>(10.0), dbGain / static_cast<SampleType>(40.0));
        const auto omega{ twoPi * (centreFrequency / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        // const auto halfSinOmega = sinOmega / static_cast<SampleType>(2.0);
        // const auto aFrac = (A + (static_cast<SampleType>(1.0) / A));
        const auto slopeFrac = (static_cast<SampleType>(1.0) / slope) - static_cast<SampleType>(1.0);
        // const auto alphaInner = (aFrac * slopeFrac) + static_cast<SampleType>(2.0);
        // const auto alpha = halfSinOmega * std::sqrt(alphaInner);
        // const auto alpha = halfSinOmega * std::sqrt(((A + (static_cast<SampleType>(1.0) / A)) * ((static_cast<SampleType>(1.0) / slope) - static_cast<SampleType>(1.0))) + static_cast<SampleType>(2.0));
        // const auto twoRootAAlpha = static_cast<SampleType>(2.0) * std::sqrt(A * alpha);
        const auto twoRootAAlpha = sinOmega * std::sqrt(((std::pow(A, static_cast<SampleType>(2.0)) + static_cast<SampleType>(1.0)) * slopeFrac) + (static_cast<SampleType>(2.0) * A));
        const auto a0 = A * ((A + static_cast<SampleType>(1.0)) - ((A - static_cast<SampleType>(1.0)) * cosOmega) + twoRootAAlpha);
        const auto a1 = static_cast<SampleType>(2.0) * A * ((A - static_cast<SampleType>(1.0)) - ((A + static_cast<SampleType>(1.0)) * cosOmega));
        const auto a2 = A * ((A + static_cast<SampleType>(1.0)) - ((A - static_cast<SampleType>(1.0)) * cosOmega) - twoRootAAlpha);
        const auto b0 = (A + static_cast<SampleType>(1.0)) + ((A - static_cast<SampleType>(1.0)) * cosOmega) + twoRootAAlpha;
        const auto b1 = static_cast<SampleType>(-2.0) * ((A - static_cast<SampleType>(1.0)) + ((A + static_cast<SampleType>(1.0)) * cosOmega));
        const auto b2 = (A + static_cast<SampleType>(1.0)) + ((A - static_cast<SampleType>(1.0)) * cosOmega) - twoRootAAlpha;
        return BiquadCoefficients<SampleType>{
            .a0 = a0,
            .a1 = a1,
            .a2 = a2,
            .b0 = b0,
            .b1 = b1,
            .b2 = b2
        };
    }

    /**
        An RBJ high shelf implementation for use with the Biquad class, based on the formulae from the RBJ Cookbook (https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html)
        \param sampleRate The sample rate to base the calculations off.
        \param centreFrequency The centre frequency of the shelf.
        \param slope The slope of the shelf in db/octave. At slope=1, the shelf is as steep as it can be. Should be between `std::numeric_limits<SampleType>::min()`, and 1`.
        \param dbGain The gain of the shelf, in decibels.
        \return An instance of BiquadCoefficients populated with the resulting coefficients.
    */
    template <FloatType SampleType>
    [[nodiscard]] BiquadCoefficients<SampleType> highShelf(double sampleRate, SampleType centreFrequency, SampleType slope, SampleType dbGain) noexcept {
        constexpr static auto twoPi = std::numbers::pi_v<SampleType> * static_cast<SampleType>(2.0);
        const auto fs{ static_cast<SampleType>(sampleRate) };
        const auto A = std::pow(static_cast<SampleType>(10.0), dbGain / static_cast<SampleType>(40.0));
        const auto omega{ twoPi * (centreFrequency / fs) };
        const auto cosOmega = std::cos(omega);
        const auto sinOmega = std::sin(omega);
        const auto alpha = (sinOmega / static_cast<SampleType>(2.0)) * std::sqrt(((A + (static_cast<SampleType>(1.0) / A)) * ((static_cast<SampleType>(1.0) / slope) - static_cast<SampleType>(1.0))) + static_cast<SampleType>(2.0));
        const auto twoRootAAlpha = static_cast<SampleType>(2.0) * std::sqrt(A * alpha);
        const auto a0 = A * ((A + static_cast<SampleType>(1.0)) + (A - static_cast<SampleType>(1.0)) * cosOmega + twoRootAAlpha);
        const auto a1 = static_cast<SampleType>(-2.0) * A * ((A - static_cast<SampleType>(1.0)) + (A + static_cast<SampleType>(1.0)) * cosOmega);
        const auto a2 = A * ((A + static_cast<SampleType>(1.0)) + (A - static_cast<SampleType>(1.0)) * cosOmega - twoRootAAlpha);
        const auto b0 = (A + static_cast<SampleType>(1.0)) - (A - static_cast<SampleType>(1.0)) * cosOmega + twoRootAAlpha;
        const auto b1 = static_cast<SampleType>(2.0) * ((A - static_cast<SampleType>(1.0)) - (A + static_cast<SampleType>(1.0)) * cosOmega);
        const auto b2 = (A + static_cast<SampleType>(1.0)) - (A - static_cast<SampleType>(1.0)) * cosOmega - twoRootAAlpha;
        return BiquadCoefficients<SampleType>{
            .a0 = a0,
            .a1 = a1,
            .a2 = a2,
            .b0 = b0,
            .b1 = b1,
            .b2 = b2
        };
    }
} // namespace marvin::dsp::filters::rbj


#endif