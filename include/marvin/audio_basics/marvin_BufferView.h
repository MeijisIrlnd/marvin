// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_BUFFER_H
#define MARVIN_BUFFER_H
#include "marvin/library/marvin_Concepts.h"
#include <span>
namespace marvin::audiobasics {
    /**
        \brief Trivially copyable view into a preallocated SampleType**.

        Useful as a lightweight and framework agnostic alternative to `xframework::AudioBuffer`.
    */
    template <FloatType SampleType>
    struct BufferView final {
        /**
            BufferView wraps around an already allocated SampleType**, and doesn't take ownership.<br>

            \param samples A pointer to the md array of samples to wrap around.
            \param nChannels The number of channels allocated (the size of the outermost array)
            \param nSamples The number of samples allocated per channel (the size of the innermost array)
        */
        BufferView(SampleType* const* samples, size_t nChannels, size_t nSamples);
        /**
            Returns the number of channels allocated in the underlying buffer
            \return The number of channels allocated.
        */
        [[nodiscard]] size_t getNumChannels() const noexcept;
        /**
            Returns the number of samples per channel allocated in the underlying buffer
            \return The number of samples allocated.
        */
        [[nodiscard]] size_t getNumSamples() const noexcept;
        /**
            Returns a read only pointer to the underlying buffer
            \return an immutable pointer to the underlying buffer
        */
        SampleType* const* getArrayOfReadPointers() const noexcept;
        /**
            Returns a mutable pointer to the underlying buffer
            \return a mutable pointer to the underlying bufffer.
        */
        SampleType* const* getArrayOfWritePointers() noexcept;
        /**
            Returns a mutable view into a specific channel of the underlying buffer.
            In debug, bounds checks with an assert
            \param channel The channel to construct a view into.
            \return a `std::span` constructed from the specified channel.
        */
        [[nodiscard]] std::span<SampleType> operator[](size_t channel) noexcept;
        /**
            Returns an immutable view into a specific channel of the underlying buffer.
            In debug, bounds checks with an assert
            \param channel The channel to construct a view into.
            \return a `std::span` constructed from the specified channel.
        */
        [[nodiscard]] std::span<const SampleType> operator[](size_t channel) const noexcept;

    private:
        SampleType* const* m_samples;
        size_t m_nChannels;
        size_t m_nSamples;
    };
} // namespace marvin::audiobasics
#endif
