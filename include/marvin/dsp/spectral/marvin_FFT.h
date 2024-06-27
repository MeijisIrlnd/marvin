// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_FFT_H
#define MARVIN_FFT_H
#include "marvin/library/marvin_Concepts.h"
#include "marvin/containers/marvin_StrideView.h"
#include <memory>
#include <span>
#include <complex>
#include <numbers>
#include <type_traits>
namespace marvin::dsp::spectral {
    /**
        \brief Enum for the available fft backends.

        Useful for verifying that the engine being used is what you expect it to be.
    */
    enum class EngineType {
        Accelerate_FFT,
        Ipp_FFT,
        Fallback_FFT
    };

    enum class FFTDirection {
        Forward,
        Inverse
    };

    enum class NormalisationType {
        None,
        One_Over_N
    };


    template <typename T>
    struct getValueType {
        using ValueType = T;
    };

    template <typename T>
    requires ComplexFloatType<T>
    struct getValueType<T> {
        using ValueType = typename T::value_type;
    };

    // template <typename T, class = void>
    // struct getValueType {
    //     using type = T;
    // };

    // template <typename T>
    // struct getValueType<T, std::void_t<typename T::value_type>> {
    //     using type = T::value_type;
    // }


    /**
        \brief Class for performing forward and inverse real only fast fourier transforms.

        The implementation is chosen at compile-time based on a few factors:
        - On macOS, will use the vDSP implementation from Accelerate.
        - On Windows, if Intel's IPP was found, will use the IPP implementation.
        - Also on Windows, if Intel's IPP was *not* found, will use the (slow and rough and ready) fallback.
    */
    template <RealOrComplexFloatType SampleType, NormalisationType Normalisation = NormalisationType::One_Over_N>
    class FFT final {
    public:
        using ValueType = getValueType<SampleType>::ValueType;

        /**
            Constructs an instance of `FFT`, with the specified order.
            The size of the FFT will be `2^order`, and the exponent is essentially there to ensure you give it a power-of-two size.
        */
        explicit FFT(size_t order);

        /**
            Because the PImpl is wrapped in a unique_ptr, we need a non-default destructor.
        */
        ~FFT() noexcept;

        /**
            Checks the FFT implementation being used. Useful for verifying the FFT is set up as expected.
            \return The engine type currently being used.
        */
        [[nodiscard]] EngineType getEngineType() const noexcept;

        /**
            Retrieves the FFT Size (`2^order` passed into the constructor).
            \return The fft size.
        */
        [[nodiscard]] size_t getFFTSize() const noexcept;

        void forward(std::span<SampleType> source, std::span<std::complex<ValueType>> dest);
        std::span<std::complex<ValueType>> forward(std::span<SampleType> source);
        void inverse(std::span<std::complex<ValueType>> source, std::span<SampleType> dest);
        std::span<SampleType> inverse(std::span<std::complex<ValueType>> source);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace marvin::dsp::spectral
#endif
