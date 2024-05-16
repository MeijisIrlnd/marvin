// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#ifndef MARVIN_FIFO_H
#define MARVIN_FIFO_H
#include <readerwriterqueue.h>
#include <optional>
#include <type_traits>
/**
    \brief Various thread safe fifos.
*/
namespace marvin::utils::fifos {
    /**
        \brief A thread-safe, realtime-safe single producer single consumer fifo.

        A wrapper around [cameron314's readerwriterqueue](https://github.com/cameron314/readerwriterqueue).<br>
        Suitable for passing data between two threads. If the queue is full, pushing will have no effect, and if the queue is empty,
        popping will return a `std::nullopt`. <br>
        `T` <b>must</b> be default-constructible, copy constructible and move constructible.<br>
        To empty the queue in a single loop:
        ```cpp
        class SomeClass {
        public:
            void emptyQueue() {
                std::optional<int> current{ };
                do {
                    current = m_fifo.tryPop();
                    if(!current) continue;
                    std::cout << "Dequeued " << *current << "\n"; // Or do something meaningful with the data here...
                } while(current);
            }
        private:
            marvin::utils::fifos::SPSC<int> m_fifo;
        };
        ```
    */
    template <typename T>
    requires std::is_copy_constructible_v<T> &&
             std::is_move_constructible_v<T> &&
             std::is_default_constructible_v<T>
    class SPSC final {
    public:
        /**
            \param maxSize The capacity of the fifo.
        */
        explicit SPSC(size_t maxSize) : m_queue(maxSize) {
        }

        /**
            Tries to pop an element from the front of the queue.
            \return std::nullopt if the queue is empty, the value at the front of the queue otherwise.
        */
        [[nodiscard]] std::optional<T> tryPop() noexcept {
            T temp{};
            const auto res = m_queue.try_dequeue(temp);
            if (!res) return {};
            return temp;
        }

        /**
            Tries to emplace an element into the back of the queue. If the queue is full, has no effect.
            Never allocates.
            \param x The value to push into the queue.
        */
        void tryPush(T&& x) noexcept {
            [[maybe_unused]] const auto res = m_queue.try_enqueue(std::move(x));
        }

        /**
            Removes all queued elements, discarding their values.
         */
        void emptyQueue() noexcept {
            auto res{ false };
            do {
                [[maybe_unused]] T curr;
                res = m_queue.try_dequeue(curr);
            } while (res);
        }

    private:
        moodycamel::ReaderWriterQueue<T> m_queue;
    };
} // namespace marvin::utils::fifos
#endif
