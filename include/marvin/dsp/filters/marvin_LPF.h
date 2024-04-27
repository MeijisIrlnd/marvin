// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_LPF_H
#define MARVIN_LPF_H
#include "marvin/library/marvin_Concepts.h"
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::dsp::filters {
    /**
        \brief A direct form i first order single pole lowpass filter.
    */
    template <FloatType SampleType>
    class LPF {
    public:
        /**
            Initialise the LPF's sample rate. If using `setCutoff` to set the coefficient, make sure to call this function before any calls to it!
            \param sampleRate The sample rate the filter should run at.
        */
        void initialise(double sampleRate) noexcept;
        /**
            Sets the desired -3dB cutoff frequency for the filter. Make sure to call `initialise` before calling this function. Internally, ends up calling `setCoeff` with the calculated coeff, from the formula
            ```
            w = f_c / f_s
            y = 1 - cos(w_c)
            coeff = -y + sqrt(y^2 + 2y)
            ```
            which is explained in more detail here: https://dsp.stackexchange.com/questions/54086/single-pole-iir-low-pass-filter-which-is-the-correct-formula-for-the-decay-coe
            <br>As the internal `coeff` variable is <b>not</b> atomic, this function needs to be either called on the audio thread, or called when the audio thread is <b>not</b> running.
            \param cutoff The cutoff frequency the LPF should use.
        */
        void setCutoff(SampleType cutoff) noexcept;
        /**
            Directly sets the coefficient the filter should use internally - see `setCutoff` for its relationship to cutoff. <br>
            As the internal `coeff` variable is <b>not</b> atomic, ensure this function is either called on the audio thread, or that the audio thread is <b>not</b> running when this function is called.
            \param newCoeff The coeff the LPF should use.
        */
        void setCoeff(SampleType newCoeff) noexcept;
        /**
            Filters a single sample. The lowpass is in the form             ```
            y[n] = ax[n] + (1-a)y[n-1]
            ```
            where a is the `coeff`.
            \param x The sample to filter.
            \return The filtered sample.
        */
        [[nodiscard]] SampleType operator()(SampleType x) noexcept;
        /**
            Resets the internal state of the filter (including zero-ing `y[n-1]`)
        */
        void reset() noexcept;

    private:
        double m_sampleRate{ 0.0 };
        SampleType m_prev{ static_cast<SampleType>(0.0) };
        SampleType m_coeff{ static_cast<SampleType>(0.0) };
    };
} // namespace marvin::dsp::filters
#include "marvin/library/marvin_DisableWarnings.h"
#endif
