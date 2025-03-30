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
#include <concurrentqueue.h>
#include <optional>
#include <type_traits>
namespace marvin::containers::fifos {
    /**
     * \brief Represents a FIFO's configuration.
     */
    enum class Type {
        /**
         * Single Producer, Single Consumer
         */
        SPSC,
        /**
         * Multi Producer, Multi Consumer
         */
        MPMC
    };

    /**
        \brief A thread-safe, realtime-safe fifo.

        Can be configured as either a single producer, single consumer queue (SPSC), a wrapper around [cameron314's readerwriterqueue](https://github.com/cameron314/readerwriterqueue),
        or a multi producer, multi consumer queue (MPMC), a wrapper around [cameron314's concurrentqueue](https://github.com/cameron314/concurrentqueue).<br>
        Suitable for passing data between threads. If the queue is full, pushing will have no effect, and if the queue is empty,
        popping will return a `std::nullopt`. <br>
        `T` <b>must</b> be default-constructible, copy constructible and move constructible.<br>
        To empty the queue in a single loop:
        ```cpp
        class SomeClass {
        public:
            void emptyQueue() {
                while(std::optional<int> current = m_fifo.tryPop()) {
                  std::cout << "Dequeued " << *current << "\n"; // Or do something meaningful with the data here...
                }
            }
        private:
            marvin::utils::fifos::SPSC<int> m_fifo;
        };
        ```
    */
    template<Type QueueType, typename T>
    requires std::is_copy_constructible_v<T> &&
             std::is_move_constructible_v<T> &&
             std::is_default_constructible_v<T>
    class FIFO final {
    public:
        /**
            \param maxSize The capacity of the fifo.
        */
        explicit FIFO(size_t maxSize) : m_queue(maxSize) {

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
            \return Whether the value was pushed into the queue.
        */
        bool tryPush(T&& x) noexcept {
            return m_queue.try_enqueue(std::move(x));
        }

        /**
            Tries to emplace an element into the back of the queue. If the queue is full, has no effect.
            Never allocates.
            \param x The value to push into the queue. Must be trivially copyable.
            \return Whether the value was pushed into the queue.
        */
        bool tryPush(const T& x) noexcept(std::is_nothrow_copy_constructible_v<T>) requires std::is_trivially_copyable_v<T> {
            return m_queue.try_enqueue(x);
        }

        /**
            Tries to construct an element in-place into the back of the queue. If the queue is full, has no effect.
            Never allocates. `T` must by trivially constructible from the types in `Args`. Can only be used on an SPSC queue.
            \param args The arguments used to construct the element in-place.
            \return Whether the value was pushed into the queue.
        */
        template<typename... Args> requires(std::is_trivially_constructible_v<T, Args...> && QueueType == Type::SPSC)
        bool tryEmplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
            return m_queue.try_emplace(std::forward<Args>(args)...);
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
        using queue_t = std::conditional_t<QueueType == Type::SPSC, moodycamel::ReaderWriterQueue<T>, moodycamel::ConcurrentQueue<T>>;
        queue_t m_queue;
    };

    template<typename T>
    using SPSC = FIFO<Type::SPSC, T>;

    template<typename T>
    using MPMC = FIFO<Type::MPMC, T>;
} // namespace marvin::containers::fifos
#endif
