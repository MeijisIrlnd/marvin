// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include <marvin/library/marvin_Concepts.h>
#include <array>
namespace marvin::containers {
    /**
        \brief A simple fixed length circular buffer.

        Very similar functionally to `marvin::dsp::DelayLine`, but with a fixed length, and no interpolation.
        Useful for a situation where you *just* need a KISS circular buffer.
        99% of the time, for a delay effect, you're much better off using `marvin::dsp::DelayLine` instead.<br>
        Usage Example:
        ```cpp
        using namespace marvin::literals;
        constexpr static auto size{ 1024 };
        constexpr static auto delay{ 4 }; // 4 samples delay

        std::vector<float> impulse(size, 0.0f);
        impulse.front() = 1.0f; // [1, 0, 0, 0, 0 ......]
        std::vector<float> delayBuffer{};
        FixedCircularBuffer<float, size + 1> circularBuffer{};
        for(auto i = 0_sz; i < size; ++i) {
            const auto delayed = circularBuffer.peek(delay);
            circularBuffer.push(impulse[i]);
            delayBuffer.emplace_back(delayed);
        }
        assert(delayBuffer[delay] == 1.0f); // [0, 0, 0, 0, 1, 0, 0 ......]
        ```
    */
    template <FloatType T, size_t N>
    class FixedCircularBuffer final {
    public:
        /**
            Constructs a FixedCircularBuffer `N` points long, and zeroes the internal buffer.
        */
        FixedCircularBuffer() {
            reset();
        }

        /**
            Emplaces a sample into the internal buffer, and increments the write index, wrapping around if necessary.
            \param toPush The value to emplace into the circular buffer.
        */
        void push(T toPush) {
            m_buffer[m_writeIndex++] = toPush;
            while (m_writeIndex >= static_cast<int>(N)) {
                m_writeIndex -= static_cast<int>(N);
            }
        }

        /**
            Retrieves an element at the specified offset from the write index into the buffer - this offset can be though of as the "delay time" in
            a delay-line context.
            \param offset The number of samples behind the write index to read from.
            \return The element at the specified offset.
        */
        [[nodiscard]] T peek(size_t offset) const {
            auto actualIndex = (m_writeIndex - static_cast<int>(offset));
            while (actualIndex < 0) {
                actualIndex += static_cast<int>(N);
            }
            const auto res = m_buffer[static_cast<size_t>(actualIndex)];
            return res;
        }

        /**
            Zeroes the internal buffer, and resets the write index back to 0.
        */
        void reset() {
            std::fill(m_buffer.begin(), m_buffer.end(), static_cast<T>(0.0));
            m_writeIndex = 0;
        }

    private:
        std::array<T, N> m_buffer;
        int m_writeIndex{ 0 };
    };
} // namespace marvin::containers