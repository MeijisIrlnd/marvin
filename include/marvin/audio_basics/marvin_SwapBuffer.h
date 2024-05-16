// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_SWAPBUFFER_H
#define MARVIN_SWAPBUFFER_H
#include <marvin/library/marvin_Concepts.h>
#include <type_traits>
#include <vector>
#include <span>
#include <cassert>

namespace marvin::audiobasics {
    /**
        \brief Convenience type to handle the concept of an "active" buffer, and a "back" buffer.

    */
    template <typename T>
    requires std::is_default_constructible_v<T> &&
             std::is_copy_constructible_v<T> &&
             std::is_move_constructible_v<T>
    class SwapBuffer final {
    public:
        /**
            Constructs a SwapBuffer without allocating any space for the internal buffers.
            If using this constructor, make sure you call `resize` before doing anything else.
        */
        SwapBuffer() = default;
        SwapBuffer(const SwapBuffer<T>& other) = default;
        SwapBuffer(SwapBuffer<T>&& other) noexcept = default;

        /**
            Constructs a SwapBuffer, and resizes both internal buffers to size `len`.
            \param len The number of elements to allocate.
        */
        explicit SwapBuffer(size_t len) {
            m_a.resize(len);
            m_b.resize(len);
        }

        /**
            Constructs a SwapBuffer, resizes both internal buffers to size `len`, and then fills
            them with `fillV`.
            \param len The number of elements to allocate.
            \param fillV The value to fill the internal buffers with.
        */
        SwapBuffer(size_t len, T fillV) {
            m_a.resize(len);
            m_b.resize(len);
            std::fill(m_a.begin(), m_a.end(), fillV);
            std::fill(m_b.begin(), m_b.end(), fillV);
        }

        ~SwapBuffer() noexcept = default;

        SwapBuffer& operator=(const SwapBuffer<T>& other) = default;
        SwapBuffer& operator=(SwapBuffer<T>&& other) noexcept = default;

        /**
            Returns the number of elements allocated in the internal buffers.
            Only returns `m_a.size()`, but in debug asserts that the two sizes match.
            \return The number of elements allocated in the internal buffers.
        */
        [[nodiscard]] size_t size() const noexcept {
            assert(m_a.size() == m_b.size());
            return m_a.size();
        }
        /**
            Preallocates `toReserve` elements in the internal buffers, to allow for
            allocation-free resizing later on, provided the size given to resize is < the reserved size.<br>
            Note that after calling this, the size of the internal buffers is unchanged - to actually set the size of the buffers,
            call `resize()`.
            \param toReserve The number of elements to allocate.
        */
        void reserve(size_t toReserve) {
            m_a.reserve(toReserve);
            m_b.reserve(toReserve);
        }

        /**
            Resizes the internal buffers to be `newSize` elements long. If enough space has not been preallocated via a call to
            `reserve`, and the previous size < `newSize`, this will allocate.
            \param newSize The new size of the internal buffers.
        */
        void resize(size_t newSize) {
            m_a.resize(newSize);
            m_b.resize(newSize);
        }

        /**
            Constructs a non-owning view to the active "readable" front buffer.
            \return A non owning view into the current front buffer.
        */
        [[nodiscard]] std::span<T> getFrontBuffer() {
            auto& buffToUse = m_aIsFrontBuffer ? m_a : m_b;
            return buffToUse;
        }

        /**
            Constructs a non-owning view into the inactive "writeable" back buffer.
            \return A non owning view into the current back buffer.
        */
        [[nodiscard]] std::span<T> getBackBuffer() {
            auto& buffToUse = m_aIsFrontBuffer ? m_b : m_a;
            return buffToUse;
        }

        /**
            Swaps the front and back buffers. Internally this flips a bool, that causes subsequent calls to `getFrontBuffer()` and `getBackBuffer()` to flip the internal buffer they're using.
            Doesn't perform any allocations, and should <b>not</b> be thought of as analogous to `std::swap`.<br>
            In practice, this should be called when you want to "commit" the data from the back buffer to the active buffer.
        */
        void swap() noexcept {
            m_aIsFrontBuffer = !m_aIsFrontBuffer;
        }


    private:
        std::vector<T> m_a;
        std::vector<T> m_b;
        bool m_aIsFrontBuffer{ true };
    };
} // namespace marvin::audiobasics
#endif
