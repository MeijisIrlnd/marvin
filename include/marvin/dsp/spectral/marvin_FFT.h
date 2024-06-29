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

    /**
        Helper tag to retrieve the typename of a `::value_type` definition within a `std::complex`. This is the general case.
     */
    template <typename T>
    struct getValueType {
        using ValueType = T;
    };

    /**
        Helper tag to retrieve the typename of a `::value_type` definition within a `std::complex`. This is the specialisation for `std::complex<>`.
     */
    template <typename T>
    requires ComplexFloatType<T>
    struct getValueType<T> {
        using ValueType = typename T::value_type;
    };

    /**
        \brief Class for performing real or complex 1D FFTs.

        The template type `SampleType` dictates whether the transform is real-only, or complex. Accepted types are `float`, `double`, `std::complex<float>`, and `std::complex<double>`.
        Performs no scaling on the forward data, and scales the inverse data by `1 / N`.

        The implementation is chosen at compile-time based on a few factors:
        - On macOS, will use the vDSP implementation from Accelerate.
        - On Windows, if Intel's IPP was found, will use the IPP implementation.
        - Also on Windows, if Intel's IPP was *not* found, will use the (slow and rough and ready) fallback.

        The fallback implementation is still a WIP, and is actively being updated. Particularly, at the time of writing it only performs radix 2 transforms.
    */
    template <RealOrComplexFloatType SampleType>
    class FFT final {
    public:
        using ValueType = typename getValueType<SampleType>::ValueType;

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

        /**
            Performs a forward transform on the data passed to `source`, and writes the results to the array-like passed to `dest`.\n
            \param source The input array-like to perform the transform on. <b>must be</b> `N` points long.
            \param dest An array like to store the results in. If `SampleType` is a real (`float` or `double`), `dest.size()` should be `(N / 2) + 1`. In this case, DC is stored in bin 0's real component, and nyquist is stored in the final bin's real component.\n If `SampleType` is a `std::complex<>`, `dest.size()` should be `N`.
         */
        void forward(std::span<SampleType> source, std::span<std::complex<ValueType>> dest);
        /**
            Performs a forward transform on the data passed to `source`, and writes the results to an internal vector preallocated to the correct size.
            \param source The input array-like to perform the transform on. <b>Must be</b> `N` points long.
            \return A non owning view into the internal result vector. Note that consecutive calls to `forward` will overwrite this data, so if it's needed, make sure to copy it between calls. The same sizing rules from the other overload of `forward` apply.
         */
        std::span<std::complex<ValueType>> forward(std::span<SampleType> source);
        /**
            Performs an inverse transform on the data passed to `source`, and writes the results to the array-like passed to `dest`.\n
            Scales the data internally by `1/N`, so if you've done any scaling yourself, be sure to revert it before calling this function.
            \param source The input array-like to perform the inverse transform on. If `SampleType` is a real (`float` or `double`), `source.size()` should be `(N / 2) + 1`, with DC is stored in bin 0's real component, and nyquist stored in the final bin's real component.\n If `SampleType` is a `std::complex<>`, `source.size()` should be `N`.
            \param dest The destination array-like to write the results to. <b>Must</b> be `N` points long.
        */
        void inverse(std::span<std::complex<ValueType>> source, std::span<SampleType> dest);
        /**
            Performs an inverse transform on the data passed to `source`, and writes the results to an internal vector preallocated to the right size.
            \param source The input array-like to perform the inverse transform on. Sizing rules from the other overload of `inverse` apply.
            \return A non owning view into the internal result vector. Note that consecutive calls to `inverse` will overwrite this data, so if it's needed, make sure to copy it between calls.
         */
        std::span<SampleType> inverse(std::span<std::complex<ValueType>> source);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace marvin::dsp::spectral
#endif
