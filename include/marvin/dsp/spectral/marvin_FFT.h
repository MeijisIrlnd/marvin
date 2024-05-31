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


    template <FloatType SampleType, int N, size_t Stride>
    void recurse(std::span<SampleType> samples, std::span<std::complex<SampleType>> dest) {
        if constexpr (N <= 1)
            return;
        else {
            constexpr static auto newStride = Stride * 2;
            recurse<SampleType, N / 2, newStride>(samples, dest);
            std::span<SampleType> odds{ samples.data() + 1, samples.size() };
            std::span<std::complex<SampleType>> oddDest{ dest.data() + 1, dest.size() };
            recurse<SampleType, N / 2, newStride>(odds, oddDest);
            marvin::containers::StrideView<SampleType, Stride> evenView{ samples };
            marvin::containers::StrideView<SampleType, Stride> oddView{ odds };
            marvin::containers::StrideView<std::complex<SampleType>, Stride> destView{ dest };
            [[maybe_unused]] const auto stride = Stride;
            [[maybe_unused]] const auto n = N;
            for (auto m = 0; m < N; ++m) {
                const auto aliasedIndex = m % (N / 2);
                const auto period = static_cast<SampleType>(m) / static_cast<SampleType>(N);
                const auto twiddle = std::exp(static_cast<SampleType>(-2.0) * std::numbers::pi_v<SampleType> * period);
                const auto res = evenView[aliasedIndex] + twiddle * oddView[aliasedIndex];
                destView[m] = res;
            }
        }
    }

    /**
        \brief Class for performing forward and inverse real only fast fourier transforms.

        The implementation is chosen at compile-time based on a few factors:
        - On macOS, will use the vDSP implementation from Accelerate.
        - On Windows, if Intel's IPP was found, will use the IPP implementation.
        - Also on Windows, if Intel's IPP was *not* found, will use the (slow and rough and ready) fallback.
    */
    template <FloatType SampleType>
    class FFT final {
    public:
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
            Performs a forward fft on the data provided in `source`. The resulting data is stored internally as a member of the
            implementation class, to save the user from having to allocate a buffer of the correct size on their end, and is returned via a
            non-owning view *into* that data. For that reason, make sure you've either copied the data to a location of your choosing, or are done using it before calling
            this function again.<br>
            \param source The data to perform the FFT on. This <b>must</b> match the fft size the class was constructed with.
            \return A non-owning view into the resulting data, consisting of `(NFFT / 2) + 1` complex numbers. DC is stored in bin 0, and Nyquist is stored in bin N.
        */
        [[nodiscard]] std::span<std::complex<SampleType>> performForwardTransform(std::span<SampleType> source);

        /**
            Performs a forward fft on the data provided in `source`, and stores the result in `dest`.
            This overload allows you to provide your own array-like to store the fft results in, but requires you to handle allocation of the correct size.
            Specifically, `dest` must be `(NFFT / 2) + 1` `std::complex<T>`-s long.
            \param source The data to perform the FFT on. This <b>must</b> match the fft size the class was constructed with.
            \param dest An array like of type `std::complex<T>`, `(NFFT / 2) + 1` points long, to store the results in.
        */
        void performForwardTransform(std::span<SampleType> source, std::span<std::complex<SampleType>> dest);

        /**
            Performs an inverse fft on the data provided in `source`. The resulting data is stored internally as a member of the implementation class,
            to save the user from having to allocate a buffer of the correct size on their end, and is returned via a non-owning view *into* that data.
           For that reason, make sure you've either copied the data to a location of your choosing, or are done using it before calling this function again.<br>
           \param source An array like of type std::complex<T>, `(NFFT / 2) + 1` points long.
           \return A non owning view into the resulting time-domain data, consisting of `NFFT` reals.
        */
        [[nodiscard]] std::span<SampleType> performInverseTransform(std::span<std::complex<SampleType>> source);

        /**
            Performs an inverse fft on the data provided in `source`, and stores the result in `dest`.
            This overload allows you to provide your own array-like to store the ifft results in, but moves the responsibility of allocating enough space to the user.
            Specifically, `dest` must be `NFFT` `T`s long.
            \param source An array-like<std::complex<T>> to perform the IFFT on. <b>Must</b> be `(NFFT / 2) + 1` points long.
            \param dest An array-like<T> `NFFT` points long, to store the results in.
        */
        void performInverseTransform(std::span<std::complex<SampleType>> source, std::span<SampleType> dest);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace marvin::dsp::spectral
#endif
