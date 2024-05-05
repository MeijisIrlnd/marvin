// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_BIQUADCOEFFICIENTS_H
#define MARVIN_BIQUADCOEFFICIENTS_H
#include "marvin/library/marvin_Concepts.h"
namespace marvin::dsp::filters {

    /**
        \brief A POD type for use with the Biquad class, and the SmoothedBiquadCoefficients class.

        Assumes the a coeffs will be treated as the numerators (zeroes) and the b coeffs will be treated as the denominators (poles)
    */
    template <FloatType SampleType>
    struct BiquadCoefficients final {
        SampleType a0{ static_cast<SampleType>(0.0) };
        SampleType a1{ static_cast<SampleType>(0.0) };
        SampleType a2{ static_cast<SampleType>(0.0) };
        SampleType b0{ static_cast<SampleType>(0.0) };
        SampleType b1{ static_cast<SampleType>(0.0) };
        SampleType b2{ static_cast<SampleType>(0.0) };

        template <FloatType U>
        friend bool operator==(const BiquadCoefficients<SampleType>& a, const BiquadCoefficients<SampleType>& b);
    };

    /**
        Checks the equality of two BiquadCoefficients objects.
        \param a A BiquadCoefficients object.
        \param b A BiquadCoefficients object.
        \return true if every coeff in a == every coeff in b, otherwise false.
    */
    template <FloatType SampleType>
    bool operator==(const BiquadCoefficients<SampleType>& a, const BiquadCoefficients<SampleType>& b) noexcept {
        return (a.a0 == b.a0) &&
               (a.a1 == b.a1) &&
               (a.a2 == b.a2) &&
               (a.b0 == b.b0) &&
               (a.b1 == b.b1) &&
               (a.b2 == b.b2);
    }

    /**
        Checks the equality of two BiquadCoefficients objects.
        \param a A BiquadCoefficients object.
        \param b A BiquadCoefficients object.
        \return true if any coeff in a != any coeff in b, otherwise false.
    */
    template <FloatType SampleType>
    bool operator!=(const BiquadCoefficients<SampleType>& a, const BiquadCoefficients<SampleType>& b) noexcept {
        return !(a == b);
    }
} // namespace marvin::dsp::filters
#endif